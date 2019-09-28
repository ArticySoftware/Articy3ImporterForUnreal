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
#include "ArticyPackage.h"

namespace ArticyImporterHelpers
{
	inline UPackage* FindOrCreatePackage(const FString Name)
	{
		const FString PackageName = ArticyHelpers::ArticyGeneratedFolder / Name;
		UPackage* AssetPackage = CreatePackage(nullptr, *PackageName);
		AssetPackage->FullyLoad();

		return AssetPackage;
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
			auto AssetPackage = FindOrCreatePackage(FileName);
			EObjectFlags Flags = RF_Public | RF_Standalone;
			AssetType* CreatedAsset = NewObject<AssetType>(AssetPackage, UClass, *ActualAssetName, Flags);;

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

		UE_LOG(LogArticyImporter, Error, TEXT("ArticyImporter: Could not find class %s!"), ClassName);

		return nullptr;
	}

	template <typename AssetType>
	AssetType* GenerateSubAsset(const TCHAR* ClassName, const TCHAR* ModuleName, const FString AssetName = "", UObject* Outer = nullptr)
	{
		if(!Outer)
		{
			UE_LOG(LogArticyImporter, Error, TEXT("Could not generate sub-asset %s: Outer is null!"), *AssetName);
			return nullptr;
		}
		
		const auto ActualAssetName = AssetName.IsEmpty() ? ClassName : AssetName;
		const auto FileName = ActualAssetName.Replace(TEXT(" "), TEXT("_"));

		auto FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), ModuleName, ClassName);
		auto UClass = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, AssetType::StaticClass());
		if (UClass)
		{
			// only public, not standalone, since the assets are bound to their outers
			EObjectFlags Flags = RF_Public;
			AssetType* CreatedAsset = NewObject<AssetType>(Outer, UClass, FName(*ActualAssetName), Flags);

			// if we successfully created the asset, notify the asset registry and mark it dirty
			if (CreatedAsset)
			{
				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(Cast<UObject>(CreatedAsset));
			}

			return CreatedAsset;
		}

		UE_LOG(LogArticyImporter, Error, TEXT("ArticyImporter: Could not find class %s!"), ClassName);

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
