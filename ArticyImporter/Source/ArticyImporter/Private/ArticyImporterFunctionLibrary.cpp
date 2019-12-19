// Fill out your copyright notice in the Description page of Project Settings.


#include "ArticyImporterFunctionLibrary.h"
#include "ArticyImporter.h"
#include "ArticyJSONFactory.h"
#include "CodeGeneration/CodeGenerator.h"

void FArticyImporterFunctionLibrary::ForceCompleteReimport(UArticyImportData* ImportData)
{
	if (!EnsureImportFile(&ImportData))
		return;

	ImportData->Settings.ObjectDefinitionsHash.Reset();
	ImportData->Settings.ScriptFragmentsHash.Reset();
	ReimportChanges(ImportData);
}

void FArticyImporterFunctionLibrary::ReimportChanges(UArticyImportData* ImportData)
{
	if (!EnsureImportFile(&ImportData))
		return;

	const auto Factory = NewObject<UArticyJSONFactory>();
	if (Factory)
	{
		Factory->Reimport(ImportData);
		//GC will destroy factory
	}
}

void FArticyImporterFunctionLibrary::RegenerateAssets(UArticyImportData* ImportData)
{
	if (!EnsureImportFile(&ImportData))
		return;

	CodeGenerator::GenerateAssets(ImportData);
}

bool FArticyImporterFunctionLibrary::EnsureImportFile(UArticyImportData** ImportData)
{
	if (!*ImportData)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(UArticyImportData::StaticClass()->GetFName(), AssetData);

		if (!AssetData.Num())
		{
			UE_LOG(LogArticyImporter, Error, TEXT("Could not found an import file."));
		}
		else
		{
			*ImportData = Cast<UArticyImportData>(AssetData[0].GetAsset());

			if (AssetData.Num() > 1)
				UE_LOG(LogArticyImporter, Error, TEXT("Found more than one import file. This is not supported by the plugin. Using the first found file for now: %s"), *AssetData[0].ObjectPath.ToString());
		}
	}

	return *ImportData != nullptr;
}
