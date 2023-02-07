//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#define JSON_SECTION_SETTINGS TEXT("Settings")
#define JSON_SECTION_PROJECT TEXT("Project")
#define JSON_SECTION_GLOBALVARS TEXT("GlobalVariables")
#define JSON_SECTION_SCRIPTMEETHODS TEXT("ScriptMethods")
#define JSON_SECTION_OBJECTDEFS TEXT("ObjectDefinitions")
#define JSON_SECTION_PACKAGES TEXT("Packages")
#define JSON_SECTION_HIERARCHY TEXT("Hierarchy")

enum class EArticyAssetCategory : uint8;
class UAD_FileData;
struct FArticyGVNamespace;

/**
 * @brief Build an AD_File object from a given Articy JSon Object
 */
class ADJsonFileBuilder
{
public:
	static void BuildAsset(UAD_FileData &FileDataAsset,const TSharedPtr<FJsonObject> RootObject);
	
private:
	// Flat data
	static void GetSettings(struct FADSettings &Settings, const TSharedPtr<FJsonObject> Json);
	static void GetProjectDefinitions(struct FADProjectDef &Project, const TSharedPtr<FJsonObject> Json);

	// Packages
	static void GetPackages(struct FADPackages &PacksList, const TArray<TSharedPtr<FJsonValue>>* Json);
	static void GetPackageDef(struct FArticyPackageDef &pDef, const TSharedPtr<FJsonObject> Json);
	static void GetModelDef(struct FArticyModelDef &modelDef,  const TSharedPtr<FJsonObject> Json);
	static EArticyAssetCategory GetAssetCategoryFromString(const FString Category);

	// Hierarchy.... Used ?
	static void GetHierarchy();

	// User methods
	static void GetUserMethods(struct FADUserMethods &userMethods, const TArray<TSharedPtr<FJsonValue>>* Json);
	static void GetScriptMethod(struct FAIDScriptMethod &scriptMethod, TSharedPtr<FJsonObject> Json, TSet<FString>& OverloadedMethods);
	
	// Global Namespaces/Variables
	static void GetGlobalVariables(struct FADGV &GV, const TArray<TSharedPtr<FJsonValue>>* Json);
	static void GetGVNameSpace(FArticyGVNamespace &GVNamespace, const TSharedPtr<FJsonObject> JsonNamespace);
	static void GetGVVariable(struct FArticyGVar& var, const TSharedPtr<FJsonObject> JsonVar);
	
	
};


