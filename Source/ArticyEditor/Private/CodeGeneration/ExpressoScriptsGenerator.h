//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyImportData.h"

/**
 * 
 */
class ExpressoScriptsGenerator
{
public:
	static void GenerateCode(const UArticyImportData* Data);
	/** Returns the filename of the generated expresso scripts class (with extension). */
	static FString GetFilename(const UArticyImportData* Data);
};
