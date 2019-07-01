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

	// struct to save the data that gets passed in upon the initial import for delayed import.
	// the InParent is a package that gets deleted since no object resides in it. That's why we save the path instead to recreate the package later on
	struct ArticyImportCreationData
	{
		UClass* InClass;
		FString PackageName;
		FName InName;
		EObjectFlags Flags;
		FString Filename;
		const TCHAR* Parms;
		FFeedbackContext* Warn;
		bool bOutOperationCanceled;

		ArticyImportCreationData(UClass* InClass, UObject* InParent,FName InName,EObjectFlags Flags,const FString& Filename, const TCHAR* Parms,FFeedbackContext* Warn,bool bOutOperationCanceled)
		{
			this->InClass = InClass;
			this->PackageName = Cast<UPackage>(InParent)->GetPathName();
			this->InName = InName;
			this->Flags = Flags;
			this->Filename = FString(*Filename);
			this->Parms = Parms;
			this->Warn = Warn;
			this->bOutOperationCanceled = bOutOperationCanceled;
		}

		ArticyImportCreationData()
		{

		}
	};
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
