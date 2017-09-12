//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

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
