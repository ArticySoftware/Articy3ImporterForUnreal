//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once
#include "AssetRegistryModule.h"

//#include "CodeGenerator.generated.h"


//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

class UArticyImportData;

class CodeGenerator
{
public:
	static void GenerateCode(UArticyImportData* Data);
	static void Recompile(UArticyImportData* Data);

	/** Returns the main source folder for all the generated code. */
	static FString GetSourceFolder();

	/** Helper methods for generated class/struct names. */
	static FString GetGeneratedInterfacesFilename(const UArticyImportData* Data);
	static FString GetGeneratedTypesFilename(const UArticyImportData* Data);
	static FString GetGlobalVarsClassname(const UArticyImportData* Data, const bool bOmittPrefix = false);
	static FString GetGVNamespaceClassname(const UArticyImportData* Data, const FString& Namespace);
	static FString GetDatabaseClassname(const UArticyImportData* Data, const bool bOmittPrefix = false);
	static FString GetMethodsProviderClassname(const UArticyImportData* Data, const bool bOmittPrefix = false);
	static FString GetExpressoScriptsClassname(const UArticyImportData* Data, const bool bOmittPrefix = false);
	static FString GetFeatureInterfaceClassName(const UArticyImportData* Data, const FArticyTemplateFeatureDef& Feature, const bool bOmittPrefix = false);

	/** Delete a file inside the source folder (empty Filename deletes the whole folder). */
	static bool DeleteGeneratedCode(const FString &Filename = "");
	static bool DeleteGeneratedAssets();

private:

	static void Compile(UArticyImportData* Data);
	static void OnCompiled(const ECompilationResult::Type Result, UArticyImportData* Data, const bool bWaitingForOtherCompile);

	//========================================//

	CodeGenerator() {}
	~CodeGenerator() {}
};
