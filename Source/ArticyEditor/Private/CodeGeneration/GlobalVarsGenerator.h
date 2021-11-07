//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

class UArticyImportData;
struct FArticyGVInfo;

/**
 * Generates the ArticyGlobalVariables struct from import data.
 */
class GlobalVarsGenerator
{
public:
	static void GenerateCode(const UArticyImportData* Data);
	static void GenerateAsset(const UArticyImportData* Data);

private:
	GlobalVarsGenerator() {}
	~GlobalVarsGenerator() {}
};
