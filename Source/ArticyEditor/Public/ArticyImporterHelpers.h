//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#pragma once

#include "Engine.h"
#include "AssetRegistryModule.h"
#include "UObject/Package.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"
#include "ArticyHelpers.h"


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
	AssetType* GenerateAsset(const TCHAR* ClassName, const TCHAR* ModuleName, const FString AssetName = "", const FString SubFolder = "", const EObjectFlags AdditionalFlags = RF_NoFlags)
	{
		const auto ActualAssetName = AssetName.IsEmpty() ? ClassName : AssetName;
		const auto FileName = (SubFolder.IsEmpty() ? ActualAssetName : SubFolder / ActualAssetName).Replace(TEXT(" "), TEXT("_"));

		auto FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), ModuleName, ClassName);
		auto UClass = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, AssetType::StaticClass());
		
		if (UClass)
		{
			auto AssetPackage = ArticyImporterHelpers::FindOrCreatePackage(FileName);
			EObjectFlags Flags = RF_Public | RF_Standalone;

			// primarily added so we can add RF_ArchetypeObject to the database and GV asset creation.
			// It fixes a problem in which the CDO would not get refreshed after reimporting changes via Hot Reload
			if(AdditionalFlags != RF_NoFlags)
			{
				Flags = Flags | AdditionalFlags;
			}
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

		//UE_LOG(LogArticyEditor, Error, TEXT("ArticyImporter: Could not find class %s!"), ClassName);

		return nullptr;
	}

	template <typename AssetType>
	AssetType* GenerateSubAsset(const TCHAR* ClassName, const TCHAR* ModuleName, const FString AssetName = "", UObject* Outer = nullptr)
	{
		if(!Outer)
		{
			//UE_LOG(LogArticyEditor, Error, TEXT("Could not generate sub-asset %s: Outer is null!"), *AssetName);
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

		//UE_LOG(LogArticyEditor, Error, TEXT("ArticyImporter: Could not find class %s!"), ClassName);

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
