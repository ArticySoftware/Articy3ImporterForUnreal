//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "DatabaseGenerator.h"
#include "CodeGenerator.h"
#include "ArticyDatabase.h"
#include "ArticyImportData.h"
#include "CodeFileGenerator.h"
#include "ExpressoScriptsGenerator.h"

void DatabaseGenerator::GenerateCode(const UArticyImportData* Data)
{
	if(!ensure(Data))
		return;

	const auto filename = CodeGenerator::GetDatabaseClassname(Data, true);
	CodeFileGenerator(filename + ".h", true, [&](CodeFileGenerator* header)
	{
		header->Line("#include \"ArticyRuntime/Public/ArticyDatabase.h\"");
		header->Line("#include \"" + ExpressoScriptsGenerator::GetFilename(Data) + "\"");
		header->Line("#include \"" + filename + ".generated.h\"");
		header->Line();

		const auto className = CodeGenerator::GetDatabaseClassname(Data);
		header->Class(className + " : public UArticyDatabase", "", true, [&]
		{
			const auto expressoClass = CodeGenerator::GetExpressoScriptsClassname(Data, false);

			header->AccessModifier("public");

			header->Method("", className, "", [&]
			{
				header->Line(FString::Printf(TEXT("SetExpressoScriptsClass(%s::StaticClass());"), *expressoClass));
			});

			{
				header->Line();
				header->Method("static " + className + "*", "Get", "const UObject* WorldContext", [&]
				{
					header->Line(FString::Printf(TEXT("return static_cast<%s*>(Super::Get(WorldContext));"), *className));
				}, "Get the instace (copy of the asset) of the database.", true,
					"BlueprintPure, Category = \"articy:draft\", meta=(HidePin=\"WorldContext\", DefaultToSelf=\"WorldContext\", DisplayName=\"GetArticyDB\", keywords=\"database\")");

				header->Line();

				const auto globalVarsClass = CodeGenerator::GetGlobalVarsClassname(Data);
				header->Method(globalVarsClass + "*", "GetGVs", "", [&]
				{
					header->Line(FString::Printf(TEXT("return static_cast<%s*>(Super::GetGVs());"), *globalVarsClass));
				}, "Get the global variables.", true,
					"BlueprintPure, Category = \"articy:draft\", meta=(keywords=\"global variables\")", "const override");
			}
		});
	});
}

UArticyDatabase* DatabaseGenerator::GenerateAsset(const UArticyImportData* Data)
{
	const auto className = CodeGenerator::GetDatabaseClassname(Data, true);
	return ArticyHelpers::GenerateAsset<UArticyDatabase>(*className, FApp::GetProjectName());
}
