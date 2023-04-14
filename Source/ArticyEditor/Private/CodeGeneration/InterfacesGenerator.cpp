//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "InterfacesGenerator.h"
#include "CodeFileGenerator.h"
#include "ImportFactories/Data/ObjectDefinitionsImport.h"
#include "ImportFactories/Data/ArticyImportData.h"

void InterfacesGenerator::GenerateCode(const UArticyImportData* Data)
{
	CodeFileGenerator(CodeGenerator::GetGeneratedInterfacesFilename(Data)+".h", true, [&](CodeFileGenerator* header)
	{
		header->Line("#include \"CoreUObject.h\"");
		if(Data->GetObjectDefs().GetFeatures().Num() > 0)
			header->Line("#include \"" + CodeGenerator::GetGeneratedInterfacesFilename(Data) + ".generated.h\"");
		header->Line();

		for (auto pair : Data->GetObjectDefs().GetFeatures())
		{
			FArticyTemplateFeatureDef feature = pair.Value;

			header->Line();
			header->UInterface(CodeGenerator::GetFeatureInterfaceClassName(Data, feature, true), 
				"MinimalAPI, BlueprintType, Category=\"" + Data->GetProject().TechnicalName + " Feature Interfaces\", meta=(CannotImplementInterfaceInBlueprint)",
				"UNINTERFACE generated from Articy " + feature.GetDisplayName() + " Feature", [&]
			{
				header->Line("public:", false, true, -1);
				header->Line();
				header->Method("virtual class " + feature.GetCppType(Data, true), "GetFeature" + feature.GetTechnicalName(), "", [&] 
				{
					header->Line("return nullptr", true);
				}, "", true, "BlueprintCallable", "const");
			});
		}
	});
}
