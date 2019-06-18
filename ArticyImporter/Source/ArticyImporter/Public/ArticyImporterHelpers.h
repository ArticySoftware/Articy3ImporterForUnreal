#pragma once

#include "Engine.h"
#include "AssetRegistryModule.h"
#include <sstream>
#include "Dom/JsonValue.h"
#include "UObject/Package.h"
#include "Dom/JsonObject.h"
#include <ObjectTools.h>
#include <AssetToolsModule.h>
#include "ArticyHelpers.h"

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
			FString objectPath = (ArticyHelpers::ArticyGeneratedFolder / fileName) + TEXT(".") + assetName;
			FAssetData existingAssetData = AssetRegistry.Get().GetAssetByObjectPath(FName(*objectPath));

			UObject* existingAsset = existingAssetData.GetAsset();

			auto AssetPackage = FindOrCreatePackage(fileName);

			EObjectFlags Flags = RF_Public | RF_Standalone;

			AssetType* createdAsset = nullptr;
			if (existingAsset)
			{
				FString packageName;
				FName tempUniqueName = MakeUniqueObjectName(existingAsset, uclass, FName(*AssetName));

				FString relativeTempPath = (SubFolder.IsEmpty() ? tempUniqueName.ToString() : SubFolder / tempUniqueName.ToString()).Replace(TEXT(" "), TEXT("_"));
				UPackage* packageOfTempObject = FindOrCreatePackage(relativeTempPath);
				createdAsset = NewObject<AssetType>(packageOfTempObject, uclass, *tempUniqueName.ToString(), Flags);

				UObject* newObject = Cast<UObject>(createdAsset);
				TArray<UObject*> ReplacementObjects;
				ReplacementObjects.Add(existingAsset);

				ObjectTools::ConsolidateObjects(newObject, ReplacementObjects, false);

				FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

				// Form a filter from the paths
				FARFilter Filter;
				Filter.bRecursivePaths = true;
				Filter.PackagePaths.Emplace(*ArticyHelpers::ArticyGeneratedFolder);
				Filter.ClassNames.Emplace(TEXT("ObjectRedirector"));

				// Query for a list of assets in the selected paths
				TArray<FAssetData> AssetList;
				AssetRegistry.Get().GetAssets(Filter, AssetList);
				TArray<UObjectRedirector*> Redirectors;

				for (FAssetData objectRedirectorData : AssetList)
				{
					UObjectRedirector* redirector = Cast<UObjectRedirector>(objectRedirectorData.GetAsset());
					if (redirector)
					{
						Redirectors.Add(redirector);
					}
				}

				AssetToolsModule.Get().FixupReferencers(Redirectors);

				FString newDirectoryName;
				GetPathToDirectoryOfAsset(newObject, newDirectoryName);

				TWeakObjectPtr<UObject> weakPtrToObject = newObject;
				FAssetRenameData renameData(weakPtrToObject, newDirectoryName, *assetName);
				TArray<FAssetRenameData> assetsToRename;
				assetsToRename.Add(renameData);

				AssetToolsModule.Get().RenameAssets(assetsToRename);
			}
			else
			{
				createdAsset = NewObject<AssetType>(AssetPackage, uclass, *assetName, Flags);
			}

			if (createdAsset)
			{
				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(Cast<UObject>(createdAsset));

				// Mark the package dirty...
				AssetPackage->MarkPackageDirty();
			}

			return createdAsset;
		}

		//NOTE: cannot use LogArticyRuntime here, causes linker error
		UE_LOG(LogTemp, Error, TEXT("ArticyImporter: Could not find class %s!"), ClassName);

		return nullptr;
	}

}
