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
	static UArticyGlobalVariables* GenerateAsset(const UArticyImportData* Data);

	/**
	 * Searches for other GlobalVariable assets (not the default one) and calls Init on them.
	 */
	static void ReinitializeOtherGlobalVariableStores(const UArticyImportData* Data);

private:
	GlobalVarsGenerator() {}
	~GlobalVarsGenerator() {}
};
