//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ObjectDefinitionsImport.h"
#include "PackagesImport.h"
#include "ArticyPackage.h"
#include "Manifest/Manifest.h"
#include "Project/Project.h"
#include "Settings/Settings.h"
#include "GlobalVariables/GVTypes.h"
#include "GlobalVariables/GVRegistry.h"
#include "Hierarchy/Hierarchy.h"
#include "UserMethods/UserMethods.h"
#include "Expresso/ExpressoFragment.h"
#include "ArticyImportData.generated.h"

/*Used as a workaround to store an array in a map*/
USTRUCT()
struct FArticyIdArray
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FArticyId> Values;
};

USTRUCT()
struct ARTICYEDITOR_API FArticyImportDataStruct
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	FADISettings Settings;
	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	FArticyProjectDef Project;
	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	FArticyGVInfo GlobalVariables;
	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	FArticyObjectDefinitions ObjectDefinitions;
	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	FArticyPackageDefs PackageDefs;
	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	FAIDUserMethods UserMethods;
	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	FADIHierarchy Hierarchy;

	UPROPERTY(VisibleAnywhere, Category = "ImportData")
	TSet<FArticyExpressoFragment> ScriptFragments;

	UPROPERTY(VisibleAnywhere, Category = "Imported")
	TArray<TSoftObjectPtr<UArticyPackage>> ImportedPackages;

	UPROPERTY(VisibleAnywhere, Category = "Imported")
	TMap<FArticyId, FArticyIdArray> ParentChildrenCache;
};

/**
 * 
 */
UCLASS()
class ARTICYEDITOR_API UArticyImportData : public UDataAsset
{
	GENERATED_BODY()

public:
	void PostInitProperties() override;

	UPROPERTY(VisibleAnywhere, Instanced, Category=ImportSettings)
	class UAssetImportData* ImportData;

	void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;

	void PostImport();

	// conventional Json import (importer < 1.6.x)
	void MainImportFromJson(const TSharedPtr<FJsonObject> RootObject);

	// ArticyX importer (importer >= 1.7.0)
	// A better architecture would have been to externalize
	// this section.. to refacto (?)
	void MainImportFromDispatcher();
	TSharedPtr<FJsonObject> _jsonManifest;
	TSharedPtr<FJsonObject> _jsonPackageDefs;
	TSharedPtr<FJsonObject> _jsonHierarchy;
	TSharedPtr<FJsonObject> _jsonUserMethods;
	TSharedPtr<FJsonObject> _jsonGlobalVars;
	TSharedPtr<FJsonObject> _jsonObjectDefinitions;
	// -------------------------------------------------
	
	const static TWeakObjectPtr<UArticyImportData> GetImportData();
	const FADISettings& GetSettings() const { return Settings; }
	FADISettings& GetSettings() { return Settings; }
	const FArticyProjectDef& GetProject() const { return Project; }
	const FArticyGVInfo& GetGlobalVars() const { return GlobalVariables; }
	const FADIHierarchy& GetHierarchy() const { return Hierarchy; }
	const FArticyObjectDefinitions& GetObjectDefs() const { return ObjectDefinitions; }
	const FArticyPackageDefs& GetPackageDefs() const { return PackageDefs; }

	TArray<TSoftObjectPtr<UArticyPackage>>& GetPackages() { return ImportedPackages; }
	TArray<UArticyPackage*> GetPackagesDirect();
	const TArray<TSoftObjectPtr<UArticyPackage>>& GetPackages() const { return ImportedPackages; }

	const TArray<FAIDScriptMethod>& GetUserMethods() const { return UserMethods.ScriptMethods; }

	void GatherScripts();
	void AddScriptFragment(const FString& Fragment, const bool bIsInstruction);
	const TSet<FArticyExpressoFragment>& GetScriptFragments() const { return ScriptFragments; }

	void AddChildToParentCache(FArticyId Parent, FArticyId Child);
	const TMap<FArticyId, FArticyIdArray>& GetParentChildrenCache() const { return ParentChildrenCache; }

	void BuildCachedVersion();
	void ResolveCachedVersion();
	bool HasCachedVersion() const { return bHasCachedVersion; }

	void SetInitialImportComplete() { bHasCachedVersion = true; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Articy")
	FArticyImportDataStruct CachedData;

	// indicates whether we've had at least one working import. Used to determine if we want to re
	UPROPERTY()
	bool bHasCachedVersion = false;

private:
	friend class FArticyEditorFunctionLibrary;

	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FADIManifest Manifest;
	
	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FADISettings Settings;
	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FArticyProjectDef Project;
	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FArticyGVInfo GlobalVariables;
	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FArticyObjectDefinitions ObjectDefinitions;
	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FArticyPackageDefs PackageDefs;
	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FAIDUserMethods UserMethods;
	UPROPERTY(VisibleAnywhere, Category="ImportData")
	FADIHierarchy Hierarchy;

	UPROPERTY(VisibleAnywhere, Category="ImportData")
	TSet<FArticyExpressoFragment> ScriptFragments;

	UPROPERTY(VisibleAnywhere, Category = "Imported")
	TArray<TSoftObjectPtr<UArticyPackage>> ImportedPackages;

	UPROPERTY(VisibleAnywhere, Category="Imported")
	TMap<FArticyId, FArticyIdArray> ParentChildrenCache;

	bool IsVariableOfType(EArticyType varType, FString FullName);
};
