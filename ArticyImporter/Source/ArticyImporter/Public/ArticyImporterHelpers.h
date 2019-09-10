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

struct ArticyCleanupData
{
	TSet<UObject*> ObjectsToDelete;
	TArray<FAssetRenameData> ObjectsToRename;
	TSet<FString> DirectoriesToDelete;
};

namespace ArticyImporterHelpers
{


	inline UPackage* FindOrCreatePackage(const FString Name)
	{
		FString PackageName = ArticyHelpers::ArticyGeneratedFolder / Name;
		UPackage* AssetPackage = CreatePackage(nullptr, *PackageName);
		AssetPackage->FullyLoad();

		return AssetPackage;
	}

	inline bool GetPathToDirectoryOfAsset(UObject* object, FString& outString)
	{
		FString pathName = object->GetOutermost()->GetPathName();
		int32 cutoffIndex = INDEX_NONE;
		pathName.FindLastChar('/', cutoffIndex);

		if (cutoffIndex != INDEX_NONE)
		{
			outString = pathName.Left(cutoffIndex);
			return true;
		}

		return false;
	}

	inline bool GetPackageNameOfDirectoryOfAsset(UObject* object, FString& outString)
	{
		FString packageName;
		GetPathToDirectoryOfAsset(object, packageName);

		int32 cutoffIndex = INDEX_NONE;
		packageName.FindLastChar('/', cutoffIndex);

		if (cutoffIndex != INDEX_NONE)
		{
			outString = packageName.RightChop(cutoffIndex);
			outString.RemoveAt(0);
			return true;
		}

		return false;
	}


	template <typename AssetType>
	AssetType* GenerateAsset(const TCHAR* ClassName, const TCHAR* ModuleName, const FString AssetName = "", const FString SubFolder = "")
	{
		const auto assetName = AssetName.IsEmpty() ? ClassName : AssetName;
		const auto fileName = (SubFolder.IsEmpty() ? assetName : SubFolder / assetName).Replace(TEXT(" "), TEXT("_"));

		auto fullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), ModuleName, ClassName);
		auto uclass = ConstructorHelpersInternal::FindOrLoadClass(fullClassName, AssetType::StaticClass());
		if (uclass)
		{
			FAssetRegistryModule& AssetRegistry = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");

			const TMap<FString, UArticyObject*> CachedExistingPackageObjects = PackagesGenerator::GetCachedExistingObjects();
			bool objectIsCached = CachedExistingPackageObjects.Contains(assetName);

			UArticyObject* existingArticyObject = objectIsCached ? *CachedExistingPackageObjects.Find(assetName) : nullptr;
			auto assetPackage = FindOrCreatePackage(fileName);
			EObjectFlags Flags = RF_Public | RF_Standalone;
			AssetType* createdAsset = nullptr;

			// if there is no existing object or there is one in the same path with the same class, just create a new asset/update the old asset
			if (!existingArticyObject || (existingArticyObject->GetPathName().Equals(ArticyHelpers::ArticyGeneratedFolder / fileName + TEXT(".") + assetName) && existingArticyObject->UObject::GetClass() == uclass))
			{
				createdAsset = NewObject<AssetType>(assetPackage, uclass, *assetName, Flags);			
			}

			// if there is an  object with the same path but a different class, we can't simply update like above with NewObject. Create the new object with a temp name  and rename late in cleanup
			else if(existingArticyObject->GetPathName().Equals(ArticyHelpers::ArticyGeneratedFolder / fileName + TEXT(".") + assetName) && existingArticyObject->UObject::GetClass() != uclass)
			{
				FName tempUniqueName = MakeUniqueObjectName(existingArticyObject, uclass, FName(*AssetName));
				FString relativeTempPath = (SubFolder.IsEmpty() ? tempUniqueName.ToString() : SubFolder / tempUniqueName.ToString()).Replace(TEXT(" "), TEXT("_"));

				assetPackage = FindOrCreatePackage(relativeTempPath);
				createdAsset = NewObject<AssetType>(assetPackage, uclass, *tempUniqueName.ToString(), Flags);
				UObject* newObject = Cast<UObject>(createdAsset);

				// cache old asset to be deleted in cleanup
				PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(existingArticyObject);

				// create rename data so that the new assets will have the old name after the cleanup
				// using temp name + rename is not required if it's a different path to begin with #TODO
				FString newDirectoryName;
				GetPathToDirectoryOfAsset(newObject, newDirectoryName);
				TWeakObjectPtr<UObject> weakPtrToObject = newObject;
				FAssetRenameData renameData(weakPtrToObject, newDirectoryName, *assetName);

				// cache the rename data to execute in cleanup
				PackagesGenerator::PostGenerationCleanupData.ObjectsToRename.Add(renameData);
			}

			// if there exists an original object but the path is different, create the new object but mark the old one to be deleted
			else
			{
				createdAsset = NewObject<AssetType>(assetPackage, uclass, *assetName, Flags);
				PackagesGenerator::PostGenerationCleanupData.ObjectsToDelete.Add(existingArticyObject);
			}

			// if we successfully created the asset, notify the asset registry and mark it dirty
			if (createdAsset)
			{
				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(Cast<UObject>(createdAsset));

				// Mark the package dirty...
				assetPackage->MarkPackageDirty();
			}

			return createdAsset;
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
