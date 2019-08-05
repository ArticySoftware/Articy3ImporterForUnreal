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

TMap<FString, UArticyObject*> PackagesGenerator::CachedArticyObjectsMapping = { {"None", nullptr} };

void PackagesGenerator::GenerateAssets(UArticyImportData* Data)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	bool bIsReimport = PackagesGenerator::CacheExistingArticyObjects();

	TArray<UArticyObject*> CachedArticyObjects;
	CachedArticyObjectsMapping.GenerateValueArray(CachedArticyObjects);

	// GetOutdatedPackageNames has to be called early since asset generation invalidates the cached objects. Maybe change to something more robust
	TArray<FString> OutdatedArticyPackageFolders = PackagesGenerator::GetOutdatedPackageNamesFromCachedObjects(Data);

	// generate new articy objects
	auto pack = Data->GetPackageDefs();
	pack.GenerateAssets(Data);

	TArray<FArticyPackage> Packages = Data->GetPackages();

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
	TArray<UObject*> ArticyObjectsToDelete;
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
			ArticyObjectsToDelete.Add(obj);
		}
	}

	// if old files exist (reimport), execute a cleanup: deletion of outdated assets and redirectors, deletion of outdated packages
	if (bIsReimport)
	{
		FString RelativeContentPath = FPaths::ProjectContentDir();
		FString FullContentPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativeContentPath);
		FString FullArticyGeneratedPackagesPath = FullContentPath / ArticyHelpers::ArticyGeneratedPackagesFolderRelativeToContent;

		// gather all objects to be force deleted
		TArray<UObject*> ObjectsToDelete;
		// append the former articy objects that were marked as to be deleted
		ObjectsToDelete.Append(ArticyObjectsToDelete);

		// gather all assets inside the outdated articy package folders to delete them - references should be fixed at this point
		for (FString folderName : OutdatedArticyPackageFolders)
		{
			TArray<FAssetData> OutdatedAssets;
			AssetRegistryModule.Get().GetAssetsByPath(FName(*(ArticyHelpers::ArticyGeneratedPackagesFolder / folderName)), OutdatedAssets, true);

			for(FAssetData data : OutdatedAssets)
			{
				ObjectsToDelete.Add(data.GetAsset());
			}
		}

		// gather all remaining redirectors that somehow survived the fix up process to delete them
		// since the prior reference fixup worked, it's fine to delete them without fixing them up anymore
		FARFilter Filter;
		Filter.bRecursivePaths = true;
		Filter.PackagePaths.Emplace(*ArticyHelpers::ArticyGeneratedFolder);
		Filter.ClassNames.Emplace(TEXT("ObjectRedirector"));

		TArray<FAssetData> RemainingRedirectors;
		AssetRegistryModule.Get().GetAssets(Filter, RemainingRedirectors);

		for(FAssetData data : RemainingRedirectors)
		{
			ObjectsToDelete.Add(data.GetAsset());
		}

		// force delete old articy assets + assets in outdated folders + objectredirectors that weren't deleted on their own
		ObjectTools::ForceDeleteObjects(ObjectsToDelete, false);

		IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
		// delete the outdated folders after having force deleted their contents
		for (FString folderName : OutdatedArticyPackageFolders)
		{
			FString fullDirectoryPath = FullArticyGeneratedPackagesPath / folderName;

			if (platformFile.DirectoryExists(*fullDirectoryPath))
			{
				platformFile.SetReadOnly(*fullDirectoryPath, false);
				platformFile.DeleteDirectory(*fullDirectoryPath);
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

bool PackagesGenerator::CacheExistingArticyObjects()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> ExistingObjectsData;
	AssetRegistryModule.Get().GetAssetsByClass(UArticyObject::StaticClass()->GetFName(), ExistingObjectsData, true);

	ClearExistingObjectsCache();

	for(FAssetData assetData : ExistingObjectsData)
	{
		UArticyObject* currentObject = Cast<UArticyObject>(assetData.GetAsset());
		CachedArticyObjectsMapping.Add(currentObject->GetName(), currentObject);
	}

	return CachedArticyObjectsMapping.Num() > 0;
}

void PackagesGenerator::ClearExistingObjectsCache()
{
	CachedArticyObjectsMapping.Empty();
}

const TMap<FString, UArticyObject*> PackagesGenerator::GetCachedExistingObjects()
{
	return CachedArticyObjectsMapping;
}

TArray<FString> PackagesGenerator::GetOutdatedPackageNamesFromCachedObjects(UArticyImportData* Data)
{
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

	return OutdatedPackageNames;
}

#undef LOCTEXT_NAMESPACE
