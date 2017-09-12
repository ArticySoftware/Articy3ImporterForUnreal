//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyImporterPrivatePCH.h"

#include "ExpressoScriptsGenerator.h"
#include "CodeFileGenerator.h"

void GenerateMethodInterface(CodeFileGenerator* header, const UArticyImportData* Data)
{
	header->UInterface(CodeGenerator::GetMethodsProviderClassname(Data, true), "", [&]
	{
		header->Line("protected:", false, true, -1);
		for(const auto method : Data->GetUserMethods())
		{
			header->Line();
			header->Method(method.GetCPPReturnType(), method.Name, method.ParameterList, nullptr, "", true, "BlueprintNativeEvent");

			auto returnOrEmpty = method.GetCPPDefaultReturn();
			if(!returnOrEmpty.IsEmpty())
				returnOrEmpty = "return " + returnOrEmpty + ";";
			header->Method("virtual " + method.GetCPPReturnType(), method.Name + "_Implementation", method.ParameterList, nullptr, "", false, "",
						   FString::Printf(TEXT("{ %s }"), *returnOrEmpty));
		}
	});
}

void GenerateUserMethods(CodeFileGenerator* header, const UArticyImportData* Data)
{
	header->Line("private:", false, true, -1);
	header->Line();

	auto iClass = "I"+CodeGenerator::GetMethodsProviderClassname(Data, true);
	for(const auto method : Data->GetUserMethods())
	{
		const bool bIsVoid = method.GetCPPReturnType() == "void";
		header->Method(method.GetCPPReturnType(), method.Name, method.ParameterList, [&]
		{
			header->Line(FString::Printf(TEXT("if(!UserMethodsProvider) return %s;"), *method.GetCPPDefaultReturn()));

			const FString returnOrEmpty = bIsVoid ? TEXT("") : TEXT("return ");
			header->Line(FString::Printf(TEXT("%s%s::Execute_%s(UserMethodsProvider%s);"), *returnOrEmpty, *iClass, *method.Name, *method.ArgumentList));
		}, "", false, "", "const");
	}
}

void GenerateExpressoScripts(CodeFileGenerator* header, const UArticyImportData* Data)
{
	header->Line("private:", false, true, -1);
	header->Line();

	/**
	 * We define all the GV namespaces as (anonymous) structs again here,
	 * so expresso scripts can just write things like:
	 *
	 *   Namespace.Variable = value;
	 *   
	 * See declaration of GlobalVariableRef for details.
	 */
	auto gvTypeName = CodeGenerator::GetGlobalVarsClassname(Data);
	for(const auto ns : Data->GetGlobalVars().Namespaces)
		header->Variable("mutable TWeakObjectPtr<" + ns.CppTypename + ">", ns.Namespace, "nullptr");

	header->Line();
	header->Method("void", "SetGV", "UArticyGlobalVariables* GV", [&]
	{
		header->Variable("auto", "gv", FString::Printf(TEXT("Cast<%s>(GV)"), *gvTypeName));
		header->Line("if(ensure(gv))");
		header->Block(true, [&]
		{
			header->Comment("Initialize all GV namespace references");
			for(const auto ns : Data->GetGlobalVars().Namespaces)
				header->Line(FString::Printf(TEXT("%s = gv->%s;"), *ns.Namespace, *ns.Namespace));
		});
	}, "", false, "", "const override");

	header->Line();
	header->Method("UClass*", "GetUserMethodsProviderInterface", "", [&]
	{
		header->Line(FString::Printf(TEXT("return %s::StaticClass();"), *CodeGenerator::GetMethodsProviderClassname(Data)));
	}, "", false, "", "override");


	header->Line();
	header->Line("public:", false, true, -1);

	header->Line();
	header->Method("", CodeGenerator::GetExpressoScriptsClassname(Data), "", [&]
	{
		const auto fragments = Data->GetScriptFragments();
		for(auto script : fragments)
		{
			if(script.OriginalFragment.IsNone())
				continue;

			auto cleanScript = script.OriginalFragment.ToString().ReplaceCharWithEscapedChar();

			if(script.bIsInstruction)
			{
				header->Line(FString::Printf(TEXT("Instructions.Add(TEXT(\"%s\"), [&]"), *cleanScript));
				header->Line("{");
				{
					header->Line(script.ParsedFragment, false, true, 1);
				}
				header->Line("});");
			}
			else
			{
				header->Line(FString::Printf(TEXT("Conditions.Add(TEXT(\"%s\"), [&]"), *cleanScript));
				header->Line("{");
				{
					//the fragment might be emtpy or contain only a comment, so we need to wrap it in
					//the ConditionOrTrue method
					header->Line("return ConditionOrTrue(", false, true, 1);
					//now comes the fragment (in next line and indented)
					header->Line(script.ParsedFragment, false, true, 2);
					//make sure there is a final semicolon
					//we put it into the next line, since the fragment might contain a line-comment
					header->Line(");", false, true, 1);
				}
				header->Line("});");
			}
		}
	});
}

void ExpressoScriptsGenerator::GenerateCode(const UArticyImportData* Data)
{
	CodeFileGenerator(GetFilename(Data), true, [&](CodeFileGenerator* header)
	{
		header->Line("#include \"ArticyRuntime/Public/ArticyExpressoScripts.h\"");
		header->Line("#include \"" + CodeGenerator::GetGlobalVarsClassname(Data, true) + ".h\"");
		header->Line("#include \"" + CodeGenerator::GetExpressoScriptsClassname(Data, true) + ".generated.h\"");

		header->Line();

		//========================================//
	
		GenerateMethodInterface(header, Data);

		header->Line();

		const auto className = CodeGenerator::GetExpressoScriptsClassname(Data);
		header->Class(className + " : public UArticyExpressoScripts", "", true, [&]
		{
			//if script support is disabled, the class remains empty
			if(Data->GetSettings().set_UseScriptSupport)
			{
				GenerateUserMethods(header, Data);
			
				header->Line();

				GenerateExpressoScripts(header, Data);
			}

		}, "BlueprintType, Blueprintable");
	});
}

FString ExpressoScriptsGenerator::GetFilename(const UArticyImportData* Data)
{
	return CodeGenerator::GetExpressoScriptsClassname(Data, true) + ".h";
}
