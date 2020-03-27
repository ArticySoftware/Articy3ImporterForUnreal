//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyEditorFunctionLibrary.h"
#include "ArticyImportData.h"

#define LOCTEXT_NAMESPACE "ArticyImporter"

// Reference: See AssetRegistryConsoleCommands.h

class FArticyEditorModule;

class FArticyEditorConsoleCommands
{
public:
	const FArticyEditorModule& Module;

	FAutoConsoleCommand ReimportCommand;

	FArticyEditorConsoleCommands(const FArticyEditorModule& InModule)
		: Module(InModule)
	
	,	ReimportCommand(
		TEXT("ArticyImporter.Reimport"),
		*LOCTEXT("CommandText_Reimport", "Reimport articy data into Unreal").ToString(),
		FConsoleCommandDelegate::CreateRaw(this, &FArticyEditorConsoleCommands::Reimport) )
	
	{}

	void Reimport()
	{
		UArticyImportData* ImportData = nullptr;
		FArticyEditorFunctionLibrary::EnsureImportDataAsset(&ImportData);
		
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