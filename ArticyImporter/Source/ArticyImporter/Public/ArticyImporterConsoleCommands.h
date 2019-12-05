#pragma once

#include "CoreMinimal.h"
#include "ArticyImporterFunctionLibrary.h"
#include "ArticyImportData.h"

#define LOCTEXT_NAMESPACE "ArticyImporter"

// Reference: See AssetRegistryConsoleCommands.h

class FArticyImporterModule;

class FArticyImporterConsoleCommands
{
public:
	const FArticyImporterModule& Module;

	FAutoConsoleCommand ReimportCommand;

	FArticyImporterConsoleCommands(const FArticyImporterModule& InModule)
		: Module(InModule)
	
	,	ReimportCommand(
		TEXT("ArticyImporter.Reimport"),
		*LOCTEXT("CommandText_Reimport", "Reimport newly exported articy data into Unreal").ToString(),
		FConsoleCommandDelegate::CreateRaw(this, &FArticyImporterConsoleCommands::Reimport) )
	
	{}

	void Reimport()
	{
		UArticyImportData* ImportData = nullptr;
		FArticyImporterFunctionLibrary::EnsureImportFile(&ImportData);
		
		if(ImportData)
		{
			const auto factory = NewObject<UArticyJSONFactory>();
			if (factory)
			{
				factory->Reimport(ImportData);
				//GC will destroy factory
			}
		}		
	}
};

#undef LOCTEXT_NAMESPACE