//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "PackagesGenerator.h"
#include "ArticyImportData.h"
#include <ObjectTools.h>
#include <AssetRegistryModule.h>
#include <AssetData.h>
#include <ModuleManager.h>
#include <AssetToolsModule.h>
#include <PlatformFilemanager.h>
#include <FileManager.h>
#include <Paths.h>
#include "ArticyHelpers.h"
#include <ObjectMacros.h>
#include <FileHelpers.h>
#include <NotificationManager.h>
#include <SNotificationList.h>
#include <MessageLog.h>
#include <Engine/BlueprintGeneratedClass.h>
#include "ArticyBaseTypes.h"

#define LOCTEXT_NAMESPACE "PackagesGenerator"

void PackagesGenerator::GenerateAssets(UArticyImportData* Data)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const bool bIsReimport = PackagesGenerator::GetCachedExistingObjects().Num() > 0;

	// generate new articy objects
	const auto ArticyPackageDefs = Data->GetPackageDefs();
	ArticyPackageDefs.GenerateAssets(Data);

	// if old files exist (reimport), prepare a cleanup: deletion of outdated assets, directories etc.
	if (bIsReimport)
	{
		TArray<FArticyPackage> Packages = Data->GetPackages();
		TArray<UArticyObject*> CachedArticyObjects;
		PackagesGenerator::GetCachedExistingObjects().GenerateValueArray(CachedArticyObjects);

		// create a map of the newly generated assets so we can compare both old and new assets
		TMap<FString, UArticyObject*> NewObjectsMapping;
		for (FArticyPackage ArticyPackage : Packages)
		{
			for (UArticyPrimitive* Prim : ArticyPackage.Objects) {
				UArticyObject* ArticyObject = Cast<UArticyObject>(Prim);
				if (ArticyObject)
				{
					NewObjectsMapping.Add(ArticyObject->GetName(), ArticyObject);
				}
			}
		}

		// gather all old articy objects that aren't contained in the import data and mark the respective assets as deleted
		for (UArticyObject* CachedArticyObject : CachedArticyObjects)
		{
			// filter out assets that were invalidated by the asset generation (assets deleted due to consolidation etc.)
			if (!CachedArticyObject->IsValidLowLevel() || !CachedArticyObject->IsA(UArticyObject::StaticClass()))
			{
				continue;
			}
			// if the asset is still valid and is not contained within the new import data (meaning it wasn't exported or it was deleted in articy draft, mark it to be deleted
			else if (!NewObjectsMapping.Contains(CachedArticyObject->GetName()))
			{
				PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(CachedArticyObject);
			}
		}
	}
}

void PackagesGenerator::CacheExistingArticyData(UArticyImportData * Data)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> ExistingObjectsData;
	AssetRegistryModule.Get().GetAssetsByClass(UArticyObject::StaticClass()->GetFName(), ExistingObjectsData, true);

	ClearCleanupData();

	for(FAssetData AssetData : ExistingObjectsData)
	{
		UArticyObject* CurrentObject = Cast<UArticyObject>(AssetData.GetAsset());
		CachedArticyObjectsMapping.Add(CurrentObject->GetName(), CurrentObject);
	}

	CacheOutdatedPackageNamesToDelete(Data);
}

void PackagesGenerator::ClearCleanupData()
{
	CachedArticyObjectsMapping.Empty();
	PackagesGenerator::PostGenerationCleanupData = {};
}

const TMap<FString, UArticyObject*> PackagesGenerator::GetCachedExistingObjects()
{
	return CachedArticyObjectsMapping;
}

void PackagesGenerator::ExecuteCleanup()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// mark all remaining assets in the outdated directories/packages to be deleted
	for (FString FolderName : PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete)
	{
		TArray<FAssetData> OutdatedAssets;
		AssetRegistryModule.Get().GetAssetsByPath(FName(*(ArticyHelpers::ArticyGeneratedPackagesFolder / FolderName)), OutdatedAssets, true);

		for (FAssetData OutdatedAsset : OutdatedAssets)
		{
			PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(OutdatedAsset.GetAsset());
		}
	}

	// force delete cached objects
	ObjectTools::ForceDeleteObjects(PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Array(), false);

	// rename cached objects (if new assets were created with the same path, but have different classes)
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().RenameAssets({ PackagesGenerator::PostGenerationCleanupData.ObjectsToRename });

	// delete directories themselves that should be empty by now
	const FString RelativeContentPath = FPaths::ProjectContentDir();
	const FString FullContentPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativeContentPath);
	const FString FullArticyGeneratedPackagesPath = FullContentPath / ArticyHelpers::ArticyGeneratedPackagesFolderRelativeToContent;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	for (FString FolderName : PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete)
	{
		FString FullDirectoryPath = FullArticyGeneratedPackagesPath / FolderName;

		if (PlatformFile.DirectoryExists(*FullDirectoryPath))
		{
			PlatformFile.SetReadOnly(*FullDirectoryPath, false);
			PlatformFile.DeleteDirectory(*FullDirectoryPath);
			AssetRegistryModule.Get().RemovePath(ArticyHelpers::ArticyGeneratedPackagesFolder / FolderName);
		}
	}


	// reset cleanup data & caches
	ClearCleanupData();
}

void PackagesGenerator::CacheOutdatedPackageNamesToDelete(UArticyImportData* Data)
{
	PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete.Empty();

	const TArray<FString> ArticyPackageFolders = Data->GetPackageDefs().GetPackageFolderNames();
	TArray<FString> OutdatedPackageNames;

	TArray<UArticyObject*> CachedObjects;
	CachedArticyObjectsMapping.GenerateValueArray(CachedObjects);

	for (UArticyObject* Object : CachedObjects)
	{
		FString PathName = Object->GetOutermost()->GetPathName();
		int32 PathCutOffIndex = INDEX_NONE;
		PathName.FindLastChar('/', PathCutOffIndex);

		if (PathCutOffIndex != INDEX_NONE)
		{
			FString DirectoryPath = PathName.Left(PathCutOffIndex);
			DirectoryPath.FindLastChar('/', PathCutOffIndex);
			FString DirectoryName = DirectoryPath.RightChop(PathCutOffIndex);

			// if the directory name of an asset is invalid, mark the directory to be deleted later on
			if (!ArticyPackageFolders.Contains(DirectoryName))
			{
				OutdatedPackageNames.AddUnique(DirectoryName);
			}
		}
	}

	PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete.Append(OutdatedPackageNames);
}

TMap<FString, UArticyObject*> PackagesGenerator::CachedArticyObjectsMapping;
FArticyCleanupData PackagesGenerator::PostGenerationCleanupData;

#undef LOCTEXT_NAMESPACE
