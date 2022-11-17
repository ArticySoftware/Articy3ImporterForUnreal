//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once


#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0 
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif
#include "UObject/Package.h"
#include "ObjectTools.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/ArticyObjectWithPosition.h"
#include "Interfaces/ArticyNode.h"

namespace ArticyImporterHelpers
{
	inline UPackage* FindOrCreatePackage(const FString Name)
	{
		const FString PackageName = ArticyHelpers::GetArticyGeneratedFolder() / Name;

		// @TODO Engine Versioning
#if ENGINE_MAJOR_VERSION >= 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 26)
		UPackage* AssetPackage = CreatePackage(*PackageName);
#else
		UPackage* AssetPackage = CreatePackage(nullptr, *PackageName);
#endif

		AssetPackage->FullyLoad();

		return AssetPackage;
	}

	/** Look for a given class in a given module.
	 *	If no module was specified, look up in ArticyRuntime and then the project itself for generated classes
	 */
	inline UClass* RetrieveClass(const FString ClassName, const FString ModuleName = "", UClass* BaseClass = UArticyObject::StaticClass())
	{
		UClass* Result = nullptr;

		if (!ModuleName.IsEmpty())
		{
			auto FullClassName = FString::Printf(TEXT("/Script/%s.%s"), *ModuleName, *ClassName);
			return ConstructorHelpersInternal::FindOrLoadClass(FullClassName, BaseClass);
		}

		FString FullClassName = FString::Printf(TEXT("/Script/%s.%s"), TEXT("ArticyRuntime"), *ClassName);
		Result = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, BaseClass);

		if (Result != nullptr)
		{
			return Result;
		}

		FullClassName = FString::Printf(TEXT("/Script/%s.%s"), FApp::GetProjectName(), *ClassName);
		Result = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, BaseClass);

		return Result;
	}

	template <typename AssetType>
	AssetType* GenerateAsset(const TCHAR* ClassName, const TCHAR* ModuleName, const FString AssetName = "", const FString SubFolder = "", const EObjectFlags AdditionalFlags = RF_NoFlags, bool MustCreate = false)
	{
		const auto ActualAssetName = AssetName.IsEmpty() ? ClassName : AssetName;
		const auto FileName = (SubFolder.IsEmpty() ? ActualAssetName : SubFolder / ActualAssetName).Replace(TEXT(" "), TEXT("_"));

		//auto FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), ModuleName, ClassName);
		auto UClass = RetrieveClass(ClassName, ModuleName, AssetType::StaticClass());

		if (UClass)
		{
			auto AssetPackage = ArticyImporterHelpers::FindOrCreatePackage(FileName);
			EObjectFlags Flags = RF_Public | RF_Standalone;

			// If this asset is flagged as MustCreate...
			if (MustCreate)
			{
				// ... Then make sure the asset doesn't already exist.
				// If it does, throw the most helpful ensure message we can and return nullptr
				AssetType* Existing = FindObject<AssetType>(AssetPackage, *ActualAssetName);
				if (!ensureAlwaysMsgf(Existing == nullptr, TEXT(
					"Warning!! Somehow you have managed to load an old Articy Asset asset (%s) when it\n"
					"_should_ have been deleted already by DeleteGeneratedAssets() in CodeGenerator.cpp.\n"
					"This is likely caused by your Source Control plugin in Unreal restoring deleted files\n"
					"unexpectedly. If you're not sure what's going on, please contact support@articy.com for\n"
					"assistance with details on how you triggered this issue.\n"
					"Loading will be cancelled to avoid crashing Unreal."
				), *ActualAssetName))
				{
					return nullptr;
				}
			}

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

	// Helper struct to sort two articy Ids based on position, if available
	// ref: BehaviorTreeEditorTypes.h
	struct FCompareArticyNodeXLocation
	{
		FORCEINLINE bool operator()(const FArticyId& A, const FArticyId& B) const
		{
			const IArticyObjectWithPosition* AObjectWithPosition = Cast<IArticyObjectWithPosition>(UArticyObject::FindAsset(A));
			const IArticyObjectWithPosition* BObjectWithPosition = Cast<IArticyObjectWithPosition>(UArticyObject::FindAsset(B));

			if (!AObjectWithPosition || !BObjectWithPosition) return true;

			const FVector2D& APos = AObjectWithPosition->GetPosition();
			const FVector2D& BPos = BObjectWithPosition->GetPosition();

			if (APos.X == BPos.X)
			{
				return APos.Y < BPos.Y;
			}

			return APos.X < BPos.X;
		}
	};

}
