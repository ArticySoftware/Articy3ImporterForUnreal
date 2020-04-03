//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

class UArticyImportData;


/**
 * 
 */
class ObjectDefinitionsGenerator
{
public:
	static void GenerateCode(const UArticyImportData* Data);

private:
	ObjectDefinitionsGenerator() {};
	~ObjectDefinitionsGenerator() {};
};
