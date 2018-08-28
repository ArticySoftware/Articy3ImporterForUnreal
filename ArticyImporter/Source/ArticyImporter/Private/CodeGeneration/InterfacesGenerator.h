//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

class UArticyImportData;

class InterfacesGenerator
{

public:
	static void GenerateCode(const UArticyImportData* Data);

private:
	InterfacesGenerator() {};
	~InterfacesGenerator() {};
};
