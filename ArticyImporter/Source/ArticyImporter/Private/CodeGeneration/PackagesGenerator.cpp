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

	// get all currently existing articy objects so we know which assets existed before the new generation
	bool bDoOldFilesExist = false;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> ExistingObjectsData;
	AssetRegistryModule.Get().GetAssetsByClass(UArticyObject::StaticClass()->GetFName(), ExistingObjectsData, true);

	TArray<FString> DirectoryPathsOfOldAssets;
	TArray<FString> DirectoryNamesOfOldAssets;

	TArray<UObject*> ReferencingObjects;

	bDoOldFilesExist = ExistingObjectsData.Num() > 0;

	TMap<FString, UArticyObject*> existingObjectsMapping;
	for (FAssetData data : ExistingObjectsData) 
	{
		UArticyObject* obj = Cast<UArticyObject>(data.GetAsset());
		existingObjectsMapping.Add(obj->GetName(), obj);
	}

	TArray<UArticyObject*> OldObjects;
	existingObjectsMapping.GenerateValueArray(OldObjects);

	// mark all previously existing articy objects to be destroyed
	TArray<UObject*> objectsToDelete;
	for (UArticyObject* obj : OldObjects) 
	{
		UObject* uobj = obj;
		objectsToDelete.Add(uobj);
	}

	// generate new articy objects
	auto pack = Data->GetPackageDefs();
	pack.GenerateAssets(Data);

	// filter out assets that were invalidated
	for(UArticyObject * obj : OldObjects)
	{
		if(!obj->IsValidLowLevel())
		{
			objectsToDelete.Remove(obj);
		}
	}

	TArray<FArticyPackage> packages = Data->GetPackages();
	TArray<FString> ArticyPackageFolders = Data->GetPackageDefs().GetPackageFolderNames();
	TArray<FString> OutdatedArticyPackageFolders;

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

	// if old files exist, delete the ones that don't exist in any package anymore
	// if a file was simply moved by means of packaging rulesets, update the references and delete the old ones
	if (bDoOldFilesExist)
	{
		for (UArticyObject* oldObject : OldObjects)
		{
			// break should be removable now
			if (!oldObject->IsValidLowLevel()) continue;
			if (newObjectsMapping.Contains(oldObject->GetName()))
			{
				// if the new objects contain the updated old object it will get deleted by consolidation or is the same so will only get updated
				objectsToDelete.Remove(oldObject);

				UArticyObject* newObject = newObjectsMapping[oldObject->GetName()];

				// if the objects are different despite having the same name/ID
				// that means the newObject was moved to another package
				// Update references and delete oldObject
				if (newObject != oldObject)
				{
					UObject* newUObject = newObject;
					UObject* oldUObject = oldObject;

					// keep track of the directories of the assets
					FString pathName = oldUObject->GetOutermost()->GetPathName();
					int32 pathCutOffIndex = INDEX_NONE;
					pathName.FindLastChar('/', pathCutOffIndex);

					if (pathCutOffIndex != INDEX_NONE)
					{
						FString directoryPath = pathName.Left(pathCutOffIndex);
						DirectoryPathsOfOldAssets.AddUnique(directoryPath);

						directoryPath.FindLastChar('/', pathCutOffIndex);
						FString directoryName = directoryPath.RightChop(pathCutOffIndex);
						DirectoryNamesOfOldAssets.AddUnique(directoryName);

						// if the directory name of an asset is invalid, mark the directory to be deleted later on
						if(!ArticyPackageFolders.Contains(directoryName))
						{
							OutdatedArticyPackageFolders.AddUnique(directoryName);
						}
					}


					TArray<UObject*> oldUObjects;
					oldUObjects.Add(oldUObject);

					// updates the references and delete the old object, replacing it with a redirector
					ObjectTools::FConsolidationResults results = ObjectTools::ConsolidateObjects(newUObject, oldUObjects, false);
				}
			}
		}

		// go through all old directories, gather the redirectors and then execute the fix up. It's a slow task!
		TArray<UObjectRedirector*> redirectors;
		for (FString directoryPath : DirectoryPathsOfOldAssets)
		{
			TArray<FAssetData> Folders;
			AssetRegistryModule.Get().GetAssetsByPath(FName(*directoryPath), Folders, true);

			for (FAssetData data : Folders)
			{
				UObjectRedirector* redirector = Cast<UObjectRedirector>(data.GetAsset());
				if (redirector)
				{
					redirectors.Add(redirector);
				}
			}
		}

		ExecuteFixUpRedirectorsInGeneratedFolder();

		// force delete all objects that did not receive an update or a new version in another package
		// #BUG somehow 1-2 assets in Maniac Manfred survive this process sometimes. Cleanup down below
		ObjectTools::ForceDeleteObjects(objectsToDelete, false);

		// Delete remaining assets (there shouldn't be any) and delete the now empty folders
		FString RelativeContentPath = FPaths::ProjectContentDir();
		FString FullContentPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativeContentPath);
		FString FullArticyGeneratedPackagesPath = FullContentPath / ArticyHelpers::ArticyGeneratedPackagesFolderRelativeToContent;

		for (FString folderName : OutdatedArticyPackageFolders)
		{
			FString FullDirectoryPath = FullArticyGeneratedPackagesPath / folderName;

			TArray<FAssetData> outdatedAssets;
			AssetRegistryModule.Get().GetAssetsByPath(FName(*(ArticyHelpers::ArticyGeneratedPackagesFolder / folderName)), outdatedAssets, true);
			ObjectTools::DeleteAssets(outdatedAssets, false);

			// delete the folder as well
			if (FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*FullDirectoryPath))
			{
				FPlatformFileManager::Get().GetPlatformFile().SetReadOnly(*FullDirectoryPath, false);
				bool bDirectoryDeleted = FPlatformFileManager::Get().GetPlatformFile().DeleteDirectory(*FullDirectoryPath);
			}
		}

		// unregister the path, to make sure
		for (FString directoryPath : DirectoryPathsOfOldAssets)
		{
			bool bPathRemoved = AssetRegistryModule.Get().RemovePath(directoryPath);
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

#undef LOCTEXT_NAMESPACE