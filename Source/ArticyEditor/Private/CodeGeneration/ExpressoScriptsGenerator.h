//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ImportFactories/ArticyImportData.h"

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
