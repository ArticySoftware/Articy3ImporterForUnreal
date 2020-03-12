//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once
#include "ArticyImportData.h"

/**
 * 
 */
class ARTICYEDITOR_API FArticyEditorFunctionLibrary
{

public:
	static void ForceCompleteReimport(UArticyImportData* = nullptr);
	static void ReimportChanges(UArticyImportData* = nullptr);
	static void RegenerateAssets(UArticyImportData* = nullptr);
	static bool EnsureImportFile(UArticyImportData**);
};
