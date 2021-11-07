//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
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
				if(method.bIsOverloadedFunction && method.OriginalParameterTypes.Num() > 0)
					displayName = FString::Printf(TEXT("%s (%s)"), *method.Name, *method.GetOriginalParametersForDisplayName());

				header->Method(method.GetCPPReturnType(), method.BlueprintName, method.GetCPPParameters(), nullptr, "", true, 
					FString::Printf(TEXT("BlueprintCallable, BlueprintNativeEvent, Category=\"Articy Methods Provider\", meta=(DisplayName=\"%s\")"), *displayName));
				header->Method("virtual " + method.GetCPPReturnType(), method.BlueprintName + "_Implementation", method.GetCPPParameters(), nullptr, "", false, "",
					FString::Printf(TEXT("{ %s }"), *returnOrEmpty));
			}
			else
			{
				header->Method("virtual " + method.GetCPPReturnType(), method.Name, method.GetCPPParameters(), nullptr, "", false, "",
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
		header->Method(method.GetCPPReturnType(), method.Name, method.GetCPPParameters(), [&]
		{
			header->Line(FString::Printf(TEXT("auto methodProvider = GetUserMethodsProviderObject();")));
			header->Line(FString::Printf(TEXT("if(!methodProvider) return %s;"), *method.GetCPPDefaultReturn()));

			const FString returnOrEmpty = bIsVoid ? TEXT("") : TEXT("return ");

			if(bCreateBlueprintableUserMethods)
			{
				FString args = "";
				if(method.ArgumentList.Num() != 0)
				{
					args = FString::Printf(TEXT(", %s"), *method.GetArguments());
				}
				header->Line(FString::Printf(TEXT("%s%s::Execute_%s(methodProvider%s);"), *returnOrEmpty, *iClass, *method.BlueprintName, *args));
			}
			else
				header->Line(FString::Printf(TEXT("%sCast<%s>(methodProvider)->%s(%s);"), *returnOrEmpty, *iClass, *method.Name, *method.GetArguments()));

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
	header->Variable("mutable TWeakObjectPtr<" + gvTypeName + ">", "ActiveGlobals", "nullptr");

	header->Line();
	header->Method("void", "SetGV", "UArticyGlobalVariables* GV", [&]
	{
		header->Variable("auto", "gv", FString::Printf(TEXT("Cast<%s>(GV)"), *gvTypeName));
		header->Comment("Initialize all GV namespace references (or nullify if we're setting to nullptr)");
		for (const auto ns : Data->GetGlobalVars().Namespaces)
			header->Line(FString::Printf(TEXT("%s = gv ? gv->%s : nullptr;"), *ns.Namespace, *ns.Namespace));

		header->Comment("Store GVs");
		header->Line("ActiveGlobals = gv;");
	}, "", false, "", "const override");

	header->Line();
	header->Method("UArticyGlobalVariables*", "GetGV", "", [&]
	{
		header->Comment("Return active global variables as set by SetGV");
		header->Line("if(ActiveGlobals.IsValid()) { return ActiveGlobals.Get(); }");
		header->Line("return nullptr;");
	}, "", false, "", "override");

	header->Line();
	header->Method("UClass*", "GetUserMethodsProviderInterface", "", [&]
	{
		header->Line(FString::Printf(TEXT("return %s::StaticClass();"), *CodeGenerator::GetMethodsProviderClassname(Data)));
	}, "", false, "", "override");

	header->Line();
	header->Method("UObject*", "GetUserMethodsProviderObject", "", [&]
	{
		header->Line("if(UserMethodsProvider)");
		header->Line("	return UserMethodsProvider;");
		header->Line("if(DefaultUserMethodsProvider.IsValid())");
		header->Line("	return DefaultUserMethodsProvider.Get();");
		header->Line("return nullptr;");
	}, "", false, "", "const");

	header->Line();
	header->Line("public:", false, true, -1);

	header->Line();
	// disable "optimization cannot be applied due to function size" compile error. This error is caused by the huge constructor when all expresso
	// scripts are added to the collection and this pragma disables the optimizations. 
	header->Line("#if !((defined(PLATFORM_PS4) && PLATFORM_PS4) || (defined(PLATFORM_PS5) && PLATFORM_PS5))");
	header->Line("#pragma warning(push)");
	header->Line("#pragma warning(disable: 4883) //<disable \"optimization cannot be applied due to function size\" compile error.");
	header->Line("#endif");
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
					//the fragment might be empty or contain only a comment, so we need to wrap it in
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
	header->Line("#if !((defined(PLATFORM_PS4) && PLATFORM_PS4) || (defined(PLATFORM_PS5) && PLATFORM_PS5))");
	header->Line("#pragma warning(pop)");
	header->Line("#endif");
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
