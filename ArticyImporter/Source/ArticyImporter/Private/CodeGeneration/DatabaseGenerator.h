//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

struct FArticyGVInfo;

/**
 * Generates the project specific Articy Database class.
 */
class DatabaseGenerator
{
public:
	static void GenerateCode(const UArticyImportData* Data);
	static class UArticyDatabase* GenerateAsset(const UArticyImportData* Data);
};
