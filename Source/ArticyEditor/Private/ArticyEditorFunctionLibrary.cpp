//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//


#include "ArticyEditorFunctionLibrary.h"
#include "ArticyEditorModule.h"
#include "ArticyJSONFactory.h"
#include "CodeGeneration/CodeGenerator.h"
#include "ObjectTools.h"
#include "FileHelpers.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

void FArticyEditorFunctionLibrary::ForceCompleteReimport(UArticyImportData* ImportData)
{
	const EImportDataEnsureResult Result = EnsureImportDataAsset(&ImportData);
	// if we generated the import data asset we will cause a full reimport, so stop here
	if (Result == EImportDataEnsureResult::Generation || Result == EImportDataEnsureResult::Failure)
	{
		return;
	}

	ImportData->Settings.ObjectDefinitionsHash.Reset();
	ImportData->Settings.ScriptFragmentsHash.Reset();
	ReimportChanges(ImportData);
}

void FArticyEditorFunctionLibrary::ReimportChanges(UArticyImportData* ImportData)
{
	const EImportDataEnsureResult Result = EnsureImportDataAsset(&ImportData);
	// if we generated the import data asset we will cause a full reimport, so stop here
	if (Result == EImportDataEnsureResult::Generation || Result == EImportDataEnsureResult::Failure)
	{
		return;
	}

	const auto Factory = NewObject<UArticyJSONFactory>();
	if (Factory)
	{
		Factory->Reimport(ImportData);
		//GC will destroy factory
	}
}

void FArticyEditorFunctionLibrary::RegenerateAssets(UArticyImportData* ImportData)
{
	const EImportDataEnsureResult Result = EnsureImportDataAsset(&ImportData);
	// if we generated the import data asset we will cause a full reimport, so stop here
	if (Result == EImportDataEnsureResult::Generation || Result == EImportDataEnsureResult::Failure)
	{
		return;
	}

	CodeGenerator::GenerateAssets(ImportData);
}

EImportDataEnsureResult FArticyEditorFunctionLibrary::EnsureImportDataAsset(UArticyImportData** ImportData)
{
	EImportDataEnsureResult Result;

	if(*ImportData)
	{
		Result = Success;
	}
	else
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(UArticyImportData::StaticClass()->GetFName(), AssetData);

		if (!AssetData.Num())
		{
			UE_LOG(LogArticyEditor, Warning, TEXT("Could not find articy import data asset. Attempting to create from .articyue4 export file"));
			*ImportData = GenerateImportDataAsset();
			*ImportData ? Result = Generation : Result = Failure;
		}
		else
		{
			*ImportData = Cast<UArticyImportData>(AssetData[0].GetAsset());
			Result = AssetRegistry;
			
			if (AssetData.Num() > 1)
				UE_LOG(LogArticyEditor, Error, TEXT("Found more than one import file. This is not supported by the plugin. Using the first found file for now: %s"), *AssetData[0].ObjectPath.ToString());
		}
	}

	return Result;
}

UArticyImportData* FArticyEditorFunctionLibrary::GenerateImportDataAsset()
{
	UArticyImportData* ImportData = nullptr;
	
	UArticyJSONFactory* Factory = NewObject<UArticyJSONFactory>();

	TArray<FString> ArticyImportFiles;
	IFileManager::Get().FindFiles(ArticyImportFiles, *FPaths::ProjectContentDir(), TEXT("articyue4"));

	if (ArticyImportFiles.Num() == 0)
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Failed creation of import data asset. No .articyue4 file found in the project's content folder. Aborting process."));
		return nullptr;
	}
	
	const FString FileName = FPaths::GetBaseFilename(ArticyImportFiles[0], false);

	const FString PackagePath = TEXT("/Game/") + FileName;

	const FString CleanedPackagePath = PackagePath.Replace(TEXT(" "), TEXT("_")).Replace(TEXT("."), TEXT("_"));
	UPackage* Outer = CreatePackage(nullptr, *CleanedPackagePath);
	Outer->FullyLoad();

	const FString FullPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir(), ArticyImportFiles[0]);
	bool bRequired = false;
	UObject* ImportDataAsset = Factory->ImportObject(UArticyImportData::StaticClass(), Outer, FName(*FPaths::GetBaseFilename(CleanedPackagePath)), EObjectFlags::RF_Standalone | EObjectFlags::RF_Public, FullPath, nullptr, bRequired);

	if (ImportDataAsset)
	{
		ImportData = Cast<UArticyImportData>(ImportDataAsset);

		// automatically save the import data asset
		TArray<UPackage*> FailedToSavePackages;
		FEditorFileUtils::PromptForCheckoutAndSave({ Outer }, false, false, &FailedToSavePackages);

		UE_LOG(LogArticyEditor, Warning, TEXT("Successfully created import data asset. Continuing process."));
	}
	else
	{
		// delete the package if we couldn't create the import asset
		ObjectTools::ForceDeleteObjects({ Outer });
		UE_LOG(LogArticyEditor, Error, TEXT("Failed creation import data asset. Aborting process."));
	}
	
	return ImportData;
}
