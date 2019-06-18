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
			auto AssetPackage = FindOrCreatePackage(fileName);
			EObjectFlags Flags = RF_Public | RF_Standalone;

			auto asset = NewObject<AssetType>(AssetPackage, uclass, *assetName, Flags);

			if (asset)
			{
				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(Cast<UObject>(asset));

				// Mark the package dirty...
				AssetPackage->MarkPackageDirty();
			}

			return asset;
		}

		//NOTE: cannot use LogArticyRuntime here, causes linker error
		UE_LOG(LogTemp, Error, TEXT("ArticyImporter: Could not find class %s!"), ClassName);

		return nullptr;
	}

}
