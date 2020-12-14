//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyImportData.h"
#include "Misc/CompilationResult.h"


class UArticyImportData;

class CodeGenerator
{
public:
	static bool GenerateCode(UArticyImportData* Data);
	static void CacheCodeFiles();
	static bool RestoreCachedFiles();
	static void GenerateAssets(UArticyImportData* Data);
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
	static void OnCompiled(UArticyImportData* Data);
	static bool ParseForError(const FString& Log);

	/** Restores the previous import session (ImportData + Code). Returns true if the restoration resulted in functioning code + import data */
	static bool RestorePreviousImport(UArticyImportData* Data, const bool& bNotifyUser = true, ECompilationResult::Type Reason = ECompilationResult::Unknown);
	// Cached files, mapped from FileName to FileContent
	static TMap<FString, FString> CachedFiles;

	//========================================//

	CodeGenerator() {}
	~CodeGenerator() {}
};