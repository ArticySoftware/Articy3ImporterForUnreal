//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyEditorFunctionLibrary.h"
#include "ArticyEditorModule.h"
#include "ArticyJSONFactory.h"
#include "CodeGeneration/CodeGenerator.h"

void FArticyEditorFunctionLibrary::ForceCompleteReimport(UArticyImportData* ImportData)
{
	if (!EnsureImportFile(&ImportData))
		return;

	ImportData->Settings.ObjectDefinitionsHash.Reset();
	ImportData->Settings.ScriptFragmentsHash.Reset();
	ReimportChanges(ImportData);
}

void FArticyEditorFunctionLibrary::ReimportChanges(UArticyImportData* ImportData)
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

void FArticyEditorFunctionLibrary::RegenerateAssets(UArticyImportData* ImportData)
{
	if (!EnsureImportFile(&ImportData))
		return;

	CodeGenerator::GenerateAssets(ImportData);
}

bool FArticyEditorFunctionLibrary::EnsureImportFile(UArticyImportData** ImportData)
{
	if (!*ImportData)
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(UArticyImportData::StaticClass()->GetFName(), AssetData);

		if (!AssetData.Num())
		{
			UE_LOG(LogArticyEditor, Error, TEXT("Could not found an import file."));
		}
		else
		{
			*ImportData = Cast<UArticyImportData>(AssetData[0].GetAsset());

			if (AssetData.Num() > 1)
				UE_LOG(LogArticyEditor, Error, TEXT("Found more than one import file. This is not supported by the plugin. Using the first found file for now: %s"), *AssetData[0].ObjectPath.ToString());
		}
	}

	return *ImportData != nullptr;
}
