//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "GlobalVarsGenerator.h"
#include "CodeGenerator.h"
#include "CodeFileGenerator.h"
#include "ArticyImportData.h"
#include "ArticyGlobalVariables.h"
#include "ArticyImporterHelpers.h"

void GlobalVarsGenerator::GenerateCode(const UArticyImportData* Data)
{
	if(!ensure(Data))
		return;

	const auto filename = CodeGenerator::GetGlobalVarsClassname(Data, true);
	CodeFileGenerator(filename + ".h", true, [&](CodeFileGenerator* header)
	{
		
		header->Line("#include \"ArticyRuntime/Public/ArticyGlobalVariables.h\"");
		header->Line("#include \"" + filename + ".generated.h\"");

		//generate all the namespaces (with comment)
		for(const auto ns : Data->GetGlobalVars().Namespaces)
		{
			header->Line();
			header->Class(ns.CppTypename + TEXT(" : public UArticyBaseVariableSet"), ns.Description, true, [&]
			{
				//generate all the variables in public section
				header->Line("public:", false, true, -1);

				for(const FArticyGVar var : ns.Variables)
					header->Variable(var.GetCPPTypeString() + "*", var.Variable, "nullptr", var.Description, true,
									FString::Printf(TEXT("VisibleAnywhere, BlueprintReadOnly, Instanced, Category=\"%s\""), *ns.Namespace));

				header->Line();

				//in the constructor, create the subobject for all the variables
				header->Method("", ns.CppTypename, "", [&]
				{
					//create subobject
					for(const auto var : ns.Variables)
						header->Line(FString::Printf(TEXT("%s = CreateDefaultSubobject<%s>(\"%s\");"), *var.Variable, *var.GetCPPTypeString(), *var.Variable));
				});

				header->Line();

				//in the Init method, call all the variable's Init method
				header->Method("void", "Init", "UArticyGlobalVariables* const Store", [&]
				{
					header->Comment("initialize the variables");

					for(const auto var : ns.Variables)
					{
						header->Line(FString::Printf(TEXT("%s->Init<%s>(this, Store, TEXT(\"%s.%s\"), %s);"), *var.Variable, *var.GetCPPTypeString(), *ns.Namespace, *var.Variable, *var.GetCPPValueString()));
						header->Line(FString::Printf(TEXT("this->Variables.Add(%s);"), *var.Variable));
					}					
				});
			});
		}

		header->Line();

		//now generate the UArticyGlobalVariables class
		const auto type = CodeGenerator::GetGlobalVarsClassname(Data, false);
		header->Class(type + " : public UArticyGlobalVariables", TEXT("Global Articy Variables"), true, [&]
		{
			header->Line("public:", false, true, -1);

			//generate all the namespaces
			for(const auto ns : Data->GetGlobalVars().Namespaces)
			{
				header->Variable(ns.CppTypename + TEXT("*"), ns.Namespace, TEXT("nullptr"), ns.Description, true,
								FString::Printf(TEXT("VisibleAnywhere, BlueprintReadOnly, Instanced, Category=\"%s\""), *ns.Namespace));
			}

			//---------------------------------------------------------------------------//
			header->Line();

			header->Method(TEXT(""), type, TEXT(""), [&]
			{
				header->Comment(TEXT("create the namespaces"));
				for(const auto ns : Data->GetGlobalVars().Namespaces)
					header->Line(FString::Printf(TEXT("%s = CreateDefaultSubobject<%s>(\"%s\");"), *ns.Namespace, *ns.CppTypename, *ns.Namespace));

				header->Line();
				header->Line("Init();");
			});

			//---------------------------------------------------------------------------//
			header->Line();

			header->Method(TEXT("void"), TEXT("Init"), TEXT(""), [&]
			{
				header->Comment(TEXT("initialize the namespaces"));
				for(const auto ns : Data->GetGlobalVars().Namespaces)
				{
					header->Line(FString::Printf(TEXT("%s->Init(this);"), *ns.Namespace));
					header->Line(FString::Printf(TEXT("this->VariableSets.Add(%s);"), *ns.Namespace));
				}
			});

			//---------------------------------------------------------------------------//
			header->Line();

			header->Method(TEXT("static ") + type + TEXT("*"), TEXT("GetDefault"), TEXT("const UObject* WorldContext"), [&]
			{
				header->Line(TEXT("return static_cast<")+type+ TEXT("*>(UArticyGlobalVariables::GetDefault(WorldContext));"));
			}, TEXT("Get the default GlobalVariables (a copy of the asset)."), true,
				TEXT("BlueprintPure, Category=\"ArticyGlobalVariables\", meta=(HidePin=\"WorldContext\", DefaultToSelf=\"WorldContext\", DisplayName=\"GetArticyGV\", keywords=\"global variables\")"));
		});
	});
}

void GlobalVarsGenerator::GenerateAsset(const UArticyImportData* Data)
{
	const auto className = CodeGenerator::GetGlobalVarsClassname(Data, true);
	ArticyImporterHelpers::GenerateAsset<UArticyGlobalVariables>(*className, TEXT("ArticyGenerated"), TEXT(""), TEXT(""), RF_ArchetypeObject);
}
