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
	bool bIsReimport = PackagesGenerator::GetCachedExistingObjects().Num() > 0;

	// generate new articy objects
	auto pack = Data->GetPackageDefs();
	pack.GenerateAssets(Data);

	// if old files exist (reimport), prepare a cleanup: deletion of outdated assets, directories etc.
	if (bIsReimport)
	{
		TArray<FArticyPackage> Packages = Data->GetPackages();
		TArray<UArticyObject*> CachedArticyObjects;
		PackagesGenerator::GetCachedExistingObjects().GenerateValueArray(CachedArticyObjects);

		// create a map of the newly generated assets so we can compare both old and new assets
		TMap<FString, UArticyObject*> NewObjectsMapping;
		for (FArticyPackage package : Packages)
		{
			for (UArticyPrimitive* prim : package.Objects) {
				UArticyObject* obj = Cast<UArticyObject>(prim);
				if (obj)
				{
					NewObjectsMapping.Add(obj->GetName(), obj);
				}
			}
		}

		// gather all old articy objects that aren't contained in the import data and mark the respective assets as deleted
		for (UArticyObject* obj : CachedArticyObjects)
		{
			// filter out assets that were invalidated by the asset generation (assets deleted due to consolidation etc.)
			if (!obj->IsValidLowLevel() || !obj->IsA(UArticyObject::StaticClass()))
			{
				continue;
			}
			// if the asset is still valid and is not contained within the new import data (meaning it wasn't exported or it was deleted in articy draft, mark it to be deleted
			else if (!NewObjectsMapping.Contains(obj->GetName()))
			{
				PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(obj);
			}
		}
	}

	// mark all generated assets dirty to save them later on
	TArray<FAssetData> GeneratedAssets;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*ArticyHelpers::ArticyGeneratedFolder), GeneratedAssets, true);

	for (FAssetData assetData : GeneratedAssets)
	{
		assetData.GetAsset()->MarkPackageDirty();
	}
}

void PackagesGenerator::CacheExistingArticyData(UArticyImportData * Data)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> ExistingObjectsData;
	AssetRegistryModule.Get().GetAssetsByClass(UArticyObject::StaticClass()->GetFName(), ExistingObjectsData, true);

	ClearCleanupData();

	for(FAssetData assetData : ExistingObjectsData)
	{
		UArticyObject* currentObject = Cast<UArticyObject>(assetData.GetAsset());
		CachedArticyObjectsMapping.Add(currentObject->GetName(), currentObject);
	}

	CacheOutdatedPackageNamesToDelete(Data);
}

void PackagesGenerator::ClearCleanupData()
{
	CachedArticyObjectsMapping.Empty();
	PostGenerationCleanupData = {};
}

const TMap<FString, UArticyObject*> PackagesGenerator::GetCachedExistingObjects()
{
	return CachedArticyObjectsMapping;
}

void PackagesGenerator::ExecuteCleanup()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// mark all remaining assets in the outdated directories/packages to be deleted
	for (FString folderName : PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete)
	{
		TArray<FAssetData> OutdatedAssets;
		AssetRegistryModule.Get().GetAssetsByPath(FName(*(ArticyHelpers::ArticyGeneratedPackagesFolder / folderName)), OutdatedAssets, true);

		for (FAssetData data : OutdatedAssets)
		{
			PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(data.GetAsset());
		}
	}

	// force delete cached objects
	ObjectTools::ForceDeleteObjects(PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Array(), false);

	// rename cached objects (if new assets were created with the same path, but have different classes)
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().RenameAssets({ PackagesGenerator::PostGenerationCleanupData.ObjectsToRename });

	// delete directories themselves that should be empty by now
	FString RelativeContentPath = FPaths::ProjectContentDir();
	FString FullContentPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativeContentPath);
	FString FullArticyGeneratedPackagesPath = FullContentPath / ArticyHelpers::ArticyGeneratedPackagesFolderRelativeToContent;

	IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();

	for (FString folderName : PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete)
	{
		FString fullDirectoryPath = FullArticyGeneratedPackagesPath / folderName;

		if (platformFile.DirectoryExists(*fullDirectoryPath))
		{
			platformFile.SetReadOnly(*fullDirectoryPath, false);
			platformFile.DeleteDirectory(*fullDirectoryPath);
		}
	}

	// reset cleanup data & caches
	ClearCleanupData();
}

void PackagesGenerator::CacheOutdatedPackageNamesToDelete(UArticyImportData* Data)
{
	PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete.Empty();

	TArray<FString> ArticyPackageFolders = Data->GetPackageDefs().GetPackageFolderNames();
	TArray<FString> OutdatedPackageNames;

	TArray<UArticyObject*> cachedObjects;
	CachedArticyObjectsMapping.GenerateValueArray(cachedObjects);

	for (UArticyObject* obj : cachedObjects)
	{
		FString pathName = obj->GetOutermost()->GetPathName();
		int32 pathCutOffIndex = INDEX_NONE;
		pathName.FindLastChar('/', pathCutOffIndex);

		if (pathCutOffIndex != INDEX_NONE)
		{
			FString directoryPath = pathName.Left(pathCutOffIndex);
			directoryPath.FindLastChar('/', pathCutOffIndex);
			FString directoryName = directoryPath.RightChop(pathCutOffIndex);

			// if the directory name of an asset is invalid, mark the directory to be deleted later on
			if (!ArticyPackageFolders.Contains(directoryName))
			{
				OutdatedPackageNames.AddUnique(directoryName);
			}
		}
	}

	PackagesGenerator::PostGenerationCleanupData.DirectoriesToDelete.Append(OutdatedPackageNames);
}

TMap<FString, UArticyObject*> PackagesGenerator::CachedArticyObjectsMapping;
ArticyImporterHelpers::ArticyCleanupData PackagesGenerator::PostGenerationCleanupData;


#undef LOCTEXT_NAMESPACE
