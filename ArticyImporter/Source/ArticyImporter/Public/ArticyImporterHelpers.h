#pragma once

#include "Engine.h"
#include "AssetRegistryModule.h"
#include <sstream>
#include "Dom/JsonValue.h"
#include "UObject/Package.h"
#include "Dom/JsonObject.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "ArticyHelpers.h"
#include "CodeGeneration/PackagesGenerator.h"

struct FArticyCleanupData
{
	TSet<UObject*> ObjectsToDelete;
	TArray<FAssetRenameData> ObjectsToRename;
	TSet<FString> DirectoriesToDelete;
};

namespace ArticyImporterHelpers
{


	inline UPackage* FindOrCreatePackage(const FString Name)
	{
		const FString PackageName = ArticyHelpers::ArticyGeneratedFolder / Name;
		UPackage* AssetPackage = CreatePackage(nullptr, *PackageName);
		AssetPackage->FullyLoad();

		return AssetPackage;
	}

	inline bool GetPathToDirectoryOfAsset(UObject* Object, FString& outString)
	{
		FString PathName = Object->GetOutermost()->GetPathName();
		int32 CutoffIndex = INDEX_NONE;
		PathName.FindLastChar('/', CutoffIndex);

		if (CutoffIndex != INDEX_NONE)
		{
			outString = PathName.Left(CutoffIndex);
			return true;
		}

		return false;
	}

	inline bool GetPackageNameOfDirectoryOfAsset(UObject* Object, FString& outString)
	{
		FString PackageName;
		GetPathToDirectoryOfAsset(Object, PackageName);

		int32 CutoffIndex = INDEX_NONE;
		PackageName.FindLastChar('/', CutoffIndex);

		if (CutoffIndex != INDEX_NONE)
		{
			outString = PackageName.RightChop(CutoffIndex);
			outString.RemoveAt(0);
			return true;
		}

		return false;
	}


	template <typename AssetType>
	AssetType* GenerateAsset(const TCHAR* ClassName, const TCHAR* ModuleName, const FString AssetName = "", const FString SubFolder = "")
	{
		const auto ActualAssetName = AssetName.IsEmpty() ? ClassName : AssetName;
		const auto FileName = (SubFolder.IsEmpty() ? ActualAssetName : SubFolder / ActualAssetName).Replace(TEXT(" "), TEXT("_"));

		auto FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), ModuleName, ClassName);
		auto UClass = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, AssetType::StaticClass());
		if (UClass)
		{
			const TMap<FString, UArticyObject*> CachedExistingPackageObjects = PackagesGenerator::GetCachedExistingObjects();
			const bool bObjectIsCached = CachedExistingPackageObjects.Contains(ActualAssetName);

			UArticyObject* CachedArticyObject = bObjectIsCached ? *CachedExistingPackageObjects.Find(ActualAssetName) : nullptr;
			auto AssetPackage = FindOrCreatePackage(FileName);
			EObjectFlags Flags = RF_Public | RF_Standalone;
			AssetType* CreatedAsset = nullptr;

			// if there is no existing object or there is one in the same path with the same class, just create a new asset/update the old asset
			if (!CachedArticyObject || (CachedArticyObject->GetPathName().Equals(ArticyHelpers::ArticyGeneratedFolder / FileName + TEXT(".") + ActualAssetName) && CachedArticyObject->UObject::GetClass() == UClass))
			{
				CreatedAsset = NewObject<AssetType>(AssetPackage, UClass, *ActualAssetName, Flags);
			}

			// if there is an  object with the same path but a different class, we can't simply update like above with NewObject. Create the new object with a temp name  and rename late in cleanup
			else if(CachedArticyObject->GetPathName().Equals(ArticyHelpers::ArticyGeneratedFolder / FileName + TEXT(".") + ActualAssetName) && CachedArticyObject->UObject::GetClass() != UClass)
			{
				FName TempUniqueName = MakeUniqueObjectName(CachedArticyObject, UClass, FName(*AssetName));
				FString RelativeTempPath = (SubFolder.IsEmpty() ? TempUniqueName.ToString() : SubFolder / TempUniqueName.ToString()).Replace(TEXT(" "), TEXT("_"));

				AssetPackage = FindOrCreatePackage(RelativeTempPath);
				CreatedAsset = NewObject<AssetType>(AssetPackage, UClass, *TempUniqueName.ToString(), Flags);
				UObject* NewObject = Cast<UObject>(CreatedAsset);

				// cache old asset to be deleted in cleanup
				PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(CachedArticyObject);

				// create rename data so that the new assets will have the old name after the cleanup
				// using temp name + rename is not required if it's a different path to begin with #TODO
				FString NewDirectoryName;
				GetPathToDirectoryOfAsset(NewObject, NewDirectoryName);
				TWeakObjectPtr<UObject> WeakObjectPtr = NewObject;
				FAssetRenameData AssetRenameData(WeakObjectPtr, NewDirectoryName, *ActualAssetName);

				// cache the rename data to execute in cleanup
				PackagesGenerator::PostGenerationCleanupData.ObjectsToRename.Add(AssetRenameData);
			}

			// if there exists an original object but the path is different, create the new object but mark the old one to be deleted
			else
			{
				CreatedAsset = NewObject<AssetType>(AssetPackage, UClass, *ActualAssetName, Flags);
				PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(CachedArticyObject);
			}

			// if we successfully created the asset, notify the asset registry and mark it dirty
			if (CreatedAsset)
			{
				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(Cast<UObject>(CreatedAsset));

				// Mark the package dirty...
				AssetPackage->MarkPackageDirty();
			}

			return CreatedAsset;
		}

		//NOTE: cannot use LogArticyRuntime here, causes linker error
		UE_LOG(LogTemp, Error, TEXT("ArticyImporter: Could not find class %s!"), ClassName);

		return nullptr;
	}

	static bool IsPlayInEditor()
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.World()->IsPlayInEditor())
			{
				return true;
			}
		}
		return false;
	}

}
