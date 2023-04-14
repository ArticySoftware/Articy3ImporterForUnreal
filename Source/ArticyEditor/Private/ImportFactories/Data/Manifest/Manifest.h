//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once
#include "FileHash.h"
#include "Manifest.generated.h"

struct FADILocalizedLanguage;

USTRUCT()
struct FADIManifest
{
	GENERATED_BODY()
	
	// FADISettings => directely read inside the main UArticyImportData class
	// FADIProject => directely read inside the main UArticyImportData class
	UPROPERTY(VisibleAnywhere, Category="Manifest")
	FADIFileHash GlobalVariablesHash;
	
	UPROPERTY(VisibleAnywhere, Category="Manifest")
	FADIFileHash ObjectDefinitionsTypeHash;

	UPROPERTY(VisibleAnywhere, Category="Manifest")
	FADIFileHash ObjectDefinitionsTextsHash;

	// @todo : mod. FArticyPackageDef (?)
	//UPROPERTY(VisibleAnywhere, Category="Manifest")
	//TArray<FArticyPackageDef> PackagesFilesHashes;
	
	UPROPERTY(VisibleAnywhere, Category="Manifest")
	FADIFileHash ScriptMethodsHash;

	UPROPERTY(VisibleAnywhere, Category="Manifest")
	FADIFileHash HierarchyHash;

	UPROPERTY(VisibleAnywhere, Category="Manifest")
	TArray<FADILocalizedLanguage> Languages;
	
	void ImportFromJson(const TSharedPtr<FJsonObject> JsonRoot);
};