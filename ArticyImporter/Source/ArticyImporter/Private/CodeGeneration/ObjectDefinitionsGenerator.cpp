//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyImporterPrivatePCH.h"

#include "ObjectDefinitionsGenerator.h"
#include "CodeFileGenerator.h"
#include "ObjectDefinitionsImport.h"
#include "ArticyImportData.h"

void ObjectDefinitionsGenerator::GenerateCode(const UArticyImportData* Data)
{
	CodeFileGenerator(CodeGenerator::GetGeneratedTypesFilename(Data)+".h", true, [&](CodeFileGenerator* header)
	{
		header->Line("#include \"CoreUObject.h\"");
		header->Line("#include \"ArticyRuntime/Public/ArticyBaseInclude.h\"");
		header->Line("#include \"" + CodeGenerator::GetGeneratedInterfacesFilename(Data) + ".h\"");
		header->Line("#include \"" + CodeGenerator::GetGeneratedTypesFilename(Data) + ".generated.h\"");
		header->Line();

		for(const auto type : Data->GetObjectDefs().GetTypes())
			type.Value.GenerateCode(*header, Data);
	});
}
