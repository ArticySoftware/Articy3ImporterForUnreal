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

bool PackagesGenerator::bEditorNeedsRestart = false;
TMap<FString, UArticyObject*> PackagesGenerator::CachedArticyObjectsMapping = { {"None", nullptr} };

void PackagesGenerator::GenerateAssets(UArticyImportData* Data)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	bool bIsReimport = PackagesGenerator::CacheExistingArticyObjects();
	//TMap<FString, UArticyObject*> existingObjectsMapping = CachedArticyObjectsMapping;
	TArray<UArticyObject*> CachedArticyObjects;
	CachedArticyObjectsMapping.GenerateValueArray(CachedArticyObjects);

	// GetOutdatedPackageNames has to be called early since asset generation invalidates the cached objects. Maybe change to something more robust
	TArray<FString> OutdatedArticyPackageFolders = PackagesGenerator::GetOutdatedPackageNamesFromCachedObjects(Data);

	// generate new articy objects
	auto pack = Data->GetPackageDefs();
	pack.GenerateAssets(Data);

	TArray<FArticyPackage> packages = Data->GetPackages();
	TArray<FString> ArticyPackageFolders = Data->GetPackageDefs().GetPackageFolderNames();

	// create a map of the newly generated assets so we can compare both old and new assets
	TMap<FString, UArticyObject*> newObjectsMapping;
	for (FArticyPackage package : packages)
	{
		TArray<UArticyPrimitive*> containedObjects = package.Objects;
		for (UArticyPrimitive* prim : containedObjects) {
			UArticyObject* obj = Cast<UArticyObject>(prim);
			if (obj)
			{
				newObjectsMapping.Add(obj->GetName(), obj);
			}
		}
	}

	// gather all old articy objects that aren't contained in the import data and mark the respective assets as deleted
	TArray<UObject*> articyObjectsToDelete;
	for (UArticyObject* obj : CachedArticyObjects)
	{
		// filter out assets that were invalidated by the asset generation (assets deleted due to consolidation etc.)
		if (!obj->IsValidLowLevel() || !obj->IsA(UArticyObject::StaticClass()))
		{
			continue;
		}
		// if the asset is still valid and is not contained within the new import data (meaning it wasn't exported/deleted in articy draft, mark it to be deleted
		else if (!newObjectsMapping.Contains(obj->GetName()))
		{
			articyObjectsToDelete.Add(obj);
		}
	}

	// if old files exist (reimport), execute a cleanup: deletion of outdated assets and redirectors, deletion of outdated packages
	if (bIsReimport)
	{
		FString RelativeContentPath = FPaths::ProjectContentDir();
		FString FullContentPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativeContentPath);
		FString FullArticyGeneratedPackagesPath = FullContentPath / ArticyHelpers::ArticyGeneratedPackagesFolderRelativeToContent;

		// gather all objects to be force deleted
		TArray<UObject*> objectsToDelete;
		// append the former articy objects that were marked as to be deleted
		objectsToDelete.Append(articyObjectsToDelete);

		// gather all assets inside the outdated articy package folders to delete them - references should be fixed at this point
		for (FString folderName : OutdatedArticyPackageFolders)
		{
			TArray<FAssetData> outdatedAssets;
			AssetRegistryModule.Get().GetAssetsByPath(FName(*(ArticyHelpers::ArticyGeneratedPackagesFolder / folderName)), outdatedAssets, true);

			for(FAssetData data : outdatedAssets)
			{
				objectsToDelete.Add(data.GetAsset());
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
			objectsToDelete.Add(data.GetAsset());
		}

		// force delete old articy assets + assets in outdated folders + objectredirectors that weren't deleted on their own
		ObjectTools::ForceDeleteObjects(objectsToDelete, false);

		// delete the outdated folders after having force deleted their contents
		for (FString folderName : OutdatedArticyPackageFolders)
		{
			FString FullDirectoryPath = FullArticyGeneratedPackagesPath / folderName;

			if (FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*FullDirectoryPath))
			{
				FPlatformFileManager::Get().GetPlatformFile().SetReadOnly(*FullDirectoryPath, false);
				bool bDirectoryDeleted = FPlatformFileManager::Get().GetPlatformFile().DeleteDirectory(*FullDirectoryPath);
			}
		}

	}

	// mark all generated assets dirty to save them later on
	TArray<FAssetData> GeneratedAssets;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*ArticyHelpers::ArticyGeneratedFolder), GeneratedAssets, true);

	for (FAssetData assetData : GeneratedAssets)
	{
		UObject* object = assetData.GetAsset();
		bool bMarkedDirty = object->MarkPackageDirty();
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

	if(CachedArticyObjectsMapping.Num() > 0)
	{
		return true;
	}

	return false;
}

void PackagesGenerator::ClearExistingObjectsCache()
{
	CachedArticyObjectsMapping.Empty();
}

const TMap<FString, UArticyObject*> PackagesGenerator::GetCachedExistingObjects()
{
	return CachedArticyObjectsMapping;
}

bool PackagesGenerator::DoesEditorNeedRestart()
{
	return bEditorNeedsRestart;
}

bool PackagesGenerator::LoadAssetsIfNeeded(const TArray<FString>& ObjectPaths, TArray<UObject*>& LoadedObjects, bool bAllowedToPromptToLoadAssets, bool bLoadRedirects)
{
	bool bAnyObjectsWereLoadedOrUpdated = false;

	// Build a list of unloaded assets
	TArray<FString> UnloadedObjectPaths;
	bool bAtLeastOneUnloadedMap = false;
	for (int32 PathIdx = 0; PathIdx < ObjectPaths.Num(); ++PathIdx)
	{
		const FString& ObjectPath = ObjectPaths[PathIdx];

		UObject* FoundObject = FindObject<UObject>(NULL, *ObjectPath);
		if (FoundObject)
		{
			LoadedObjects.Add(FoundObject);
		}
		else
		{
			// Unloaded asset, we will load it later
			UnloadedObjectPaths.Add(ObjectPath);
			if (FEditorFileUtils::IsMapPackageAsset(ObjectPath))
			{
				bAtLeastOneUnloadedMap = true;
			}
		}
	}

	// Make sure all selected objects are loaded, where possible
	if (UnloadedObjectPaths.Num() > 0)
	{
		// Get the maximum objects to load before displaying the slow task
		const bool bShowProgressDialog = (UnloadedObjectPaths.Num() > 50) || bAtLeastOneUnloadedMap;
		//const bool bShowProgressDialog = (UnloadedObjectPaths.Num() > GetDefault<UContentBrowserSettings>()->NumObjectsToLoadBeforeWarning) || bAtLeastOneUnloadedMap;
		FScopedSlowTask SlowTask(UnloadedObjectPaths.Num(), LOCTEXT("LoadingObjects", "Loading Objects..."));
		if (bShowProgressDialog)
		{
			SlowTask.MakeDialog();
		}

		GIsEditorLoadingPackage = true;

		// We usually don't want to follow redirects when loading objects for the Content Browser.  It would
		// allow a user to interact with a ghost/unverified asset as if it were still alive.
		// This can be overridden by providing bLoadRedirects = true as a parameter.
		const ELoadFlags LoadFlags = bLoadRedirects ? LOAD_None : LOAD_NoRedirects;

		bool bSomeObjectsFailedToLoad = false;
		for (int32 PathIdx = 0; PathIdx < UnloadedObjectPaths.Num(); ++PathIdx)
		{
			const FString& ObjectPath = UnloadedObjectPaths[PathIdx];
			SlowTask.EnterProgressFrame(1, FText::Format(LOCTEXT("LoadingObjectf", "Loading {0}..."), FText::FromString(ObjectPath)));

			// Load up the object
			UObject* LoadedObject = LoadObject<UObject>(NULL, *ObjectPath, NULL, LoadFlags, NULL);
			if (LoadedObject)
			{
				LoadedObjects.Add(LoadedObject);
			}
			else
			{
				bSomeObjectsFailedToLoad = true;
			}

			if (GWarn->ReceivedUserCancel())
			{
				// If the user has canceled stop loading the remaining objects. We don't add the remaining objects to the failed string,
				// this would only result in launching another dialog when by their actions the user clearly knows not all of the 
				// assets will have been loaded.
				break;
			}
		}
		GIsEditorLoadingPackage = false;

		if (bSomeObjectsFailedToLoad)
		{
			FNotificationInfo Info(LOCTEXT("LoadObjectFailed", "Failed to load assets"));
			Info.ExpireDuration = 5.0f;
			Info.Hyperlink = FSimpleDelegate::CreateStatic([]() { FMessageLog("LoadErrors").Open(EMessageSeverity::Info, true); });
			Info.HyperlinkText = LOCTEXT("LoadObjectHyperlink", "Show Message Log");

			FSlateNotificationManager::Get().AddNotification(Info);
			return false;
		}
	}

	return true;
}

void PackagesGenerator::ExecuteFixUpRedirectorsInGeneratedFolder()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	// Form a filter from the paths
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace(*ArticyHelpers::ArticyGeneratedFolder);
	Filter.ClassNames.Emplace(TEXT("ObjectRedirector"));

	// Query for a list of assets in the selected paths
	TArray<FAssetData> AssetList;
	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

	if (AssetList.Num() > 0)
	{
		TArray<FString> ObjectPaths;
		for (const auto& Asset : AssetList)
		{
			ObjectPaths.Add(Asset.ObjectPath.ToString());
		}

		TArray<UObject*> Objects;
		const bool bAllowedToPromptToLoadAssets = true;
		const bool bLoadRedirects = true;
		if (LoadAssetsIfNeeded(ObjectPaths, Objects, bAllowedToPromptToLoadAssets, bLoadRedirects))
		{
			// Transform Objects array to ObjectRedirectors array
			TArray<UObjectRedirector*> Redirectors;
			for (auto Object : Objects)
			{
				auto Redirector = CastChecked<UObjectRedirector>(Object);
				Redirectors.Add(Redirector);
			}

			// Load the asset tools module
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
			AssetToolsModule.Get().FixupReferencers(Redirectors);
		}
	}
}

UClass* PackagesGenerator::LoadClassFromPath(const FString& Path)
{
	if (Path == "") return nullptr;

	return StaticLoadClass(UObject::StaticClass(), nullptr, *Path, nullptr, LOAD_None, nullptr);
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
			//DirectoryPathsOfOldAssets.AddUnique(directoryPath);

			directoryPath.FindLastChar('/', pathCutOffIndex);
			FString directoryName = directoryPath.RightChop(pathCutOffIndex);
			//DirectoryNamesOfOldAssets.AddUnique(directoryName);

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