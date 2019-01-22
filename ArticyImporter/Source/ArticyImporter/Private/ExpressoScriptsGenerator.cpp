//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ExpressoScriptsGenerator.h"
#include "CodeFileGenerator.h"
#include "ArticyPluginSettings.h"

void GenerateMethodInterface(CodeFileGenerator* header, const UArticyImportData* Data, bool bCreateBlueprintableUserMethods)
{
	header->UInterface(CodeGenerator::GetMethodsProviderClassname(Data, true), "Blueprintable", "", [&]
	{
		header->Line("public:", false, true, -1);
		for(const auto method : Data->GetUserMethods())
		{
			auto returnOrEmpty = method.GetCPPDefaultReturn();
			if(!returnOrEmpty.IsEmpty())
				returnOrEmpty = "return " + returnOrEmpty + ";";

			header->Line();

			if(bCreateBlueprintableUserMethods)
			{
				FString displayName = method.Name;
				if(method.bIsOverloadedFunction && method.OrigininalParameterTypes.Len() > 0)
					displayName = FString::Printf(TEXT("%s (%s)"), *method.Name, *method.OrigininalParameterTypes);

				header->Method(method.GetCPPReturnType(), method.BlueprintName, method.ParameterList, nullptr, "", true, 
					FString::Printf(TEXT("BlueprintCallable, BlueprintNativeEvent, meta=(DisplayName=\"%s\")"), *displayName));
				header->Method("virtual " + method.GetCPPReturnType(), method.BlueprintName + "_Implementation", method.ParameterList, nullptr, "", false, "",
					FString::Printf(TEXT("{ %s }"), *returnOrEmpty));
			}
			else
			{
				header->Method("virtual " + method.GetCPPReturnType(), method.Name, method.ParameterList, nullptr, "", false, "",
					FString::Printf(TEXT("{ %s }"), *returnOrEmpty));
			}
		}
	});
}

void GenerateUserMethods(CodeFileGenerator* header, const UArticyImportData* Data, bool bCreateBlueprintableUserMethods)
{
	header->Line("private:", false, true, -1);
	header->Line();

	auto iClass = "I"+CodeGenerator::GetMethodsProviderClassname(Data, true);
	for(const auto method : Data->GetUserMethods())
	{
		const bool bIsVoid = method.GetCPPReturnType() == "void";
		header->Method(method.GetCPPReturnType(), method.Name, method.ParameterList, [&]
		{
			header->Line(FString::Printf(TEXT("auto methodProvider = GetUserMethodsProviderObject();")));
			header->Line(FString::Printf(TEXT("if(!methodProvider) return %s;"), *method.GetCPPDefaultReturn()));

			const FString returnOrEmpty = bIsVoid ? TEXT("") : TEXT("return ");

			if(bCreateBlueprintableUserMethods)
			{
				FString args = "";
				if(!method.ArgumentList.IsEmpty())
				{
					args = FString::Printf(TEXT(", %s"), *method.ArgumentList);
				}
				header->Line(FString::Printf(TEXT("%s%s::Execute_%s(methodProvider%s);"), *returnOrEmpty, *iClass, *method.BlueprintName, *args));
			}
			else
				header->Line(FString::Printf(TEXT("%sCast<%s>(methodProvider)->%s(%s);"), *returnOrEmpty, *iClass, *method.Name, *method.ArgumentList));

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

	auto methodsProviderClassname = CodeGenerator::GetMethodsProviderClassname(Data);

	header->Line();
	header->Method("UClass*", "GetUserMethodsProviderInterface", "", [&]
	{
		header->Line(FString::Printf(TEXT("return %s::StaticClass();"), *methodsProviderClassname));
	}, "", false, "", "override");

	header->Line();
	header->Method("UObject*", "GetUserMethodsProviderObject", "", [&]
	{
		header->Line("if(UserMethodsProvider)");
		header->Line("	return UserMethodsProvider;");
		header->Line("if(DefaultUserMethodsProvider)");
		header->Line("	return DefaultUserMethodsProvider;");
		header->Line("return nullptr;");
	}, "", false, "", "const");

	header->Line();
	header->Line("public:", false, true, -1);

	header->Line();
	header->Method("", CodeGenerator::GetExpressoScriptsClassname(Data), "", [&]
	{
		const auto fragments = Data->GetScriptFragments();
		for(auto script : fragments)
		{
			if(script.OriginalFragment.IsEmpty())
				continue;


			int cleanScriptHash = GetTypeHash(script.OriginalFragment);

			if(script.bIsInstruction)
			{
				header->Line(FString::Printf(TEXT("Instructions.Add(%d, [&]"), cleanScriptHash));
				header->Line("{");
				{
					header->Line(script.ParsedFragment, false, true, 1);
				}
				header->Line("});");
			}
			else
			{
				header->Line(FString::Printf(TEXT("Conditions.Add(%d, [&]"), cleanScriptHash));
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
	// Determine if we want to make the user methods blueprintable.
	// (if true, we use a different naming to allow something like overloaded functions)
	bool bCreateBlueprintableUserMethods = UArticyPluginSettings::Get()->bCreateBlueprintTypeForScriptMethods;

	CodeFileGenerator(GetFilename(Data), true, [&](CodeFileGenerator* header)
	{
		header->Line("#include \"ArticyRuntime/Public/ArticyExpressoScripts.h\"");
		header->Line("#include \"" + CodeGenerator::GetGlobalVarsClassname(Data, true) + ".h\"");
		header->Line("#include \"" + CodeGenerator::GetExpressoScriptsClassname(Data, true) + ".generated.h\"");

		header->Line();

		//========================================//
	
		GenerateMethodInterface(header, Data, bCreateBlueprintableUserMethods);

		header->Line();

		const auto className = CodeGenerator::GetExpressoScriptsClassname(Data);
		header->Class(className + " : public UArticyExpressoScripts", "", true, [&]
		{
			//if script support is disabled, the class remains empty
			if(Data->GetSettings().set_UseScriptSupport)
			{
				GenerateUserMethods(header, Data, bCreateBlueprintableUserMethods);
			
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
