//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once
#include "ArticyImportData.h"

/**
 * 
 */
enum EImportDataEnsureResult
{
	// found the import data in the asset registry
	AssetRegistry,
	// generated import data asset
	Generation,
	// the parameter was already valid
	Success,
	// didn't find the asset in asset registry nor was it generated
	Failure
};
class ARTICYEDITOR_API FArticyEditorFunctionLibrary
{
	
public:
	static void ForceCompleteReimport(UArticyImportData* = nullptr);
	static void ReimportChanges(UArticyImportData* = nullptr);
	static void RegenerateAssets(UArticyImportData* = nullptr);
	static EImportDataEnsureResult EnsureImportDataAsset(UArticyImportData**);

private:
	static UArticyImportData* GenerateImportDataAsset();
};
