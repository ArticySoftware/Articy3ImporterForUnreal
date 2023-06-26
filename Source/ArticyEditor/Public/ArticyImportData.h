//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ObjectDefinitionsImport.h"
#include "PackagesImport.h"
#include "ArticyPackage.h"
#include "ArticyImportData.generated.h"

class UArticyImportData;

/**
 * The Settings object in the .json file.
 */
USTRUCT()
struct FADISettings
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString set_TextFormatter = "";
	
	/** If this is false, no ExpressoScripts class is generated, and script fragments are not evaluated/executed. */
	UPROPERTY(VisibleAnywhere, Category="Settings")
	bool set_UseScriptSupport = false;
	
	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ExportVersion = "";

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString GlobalVariablesHash = "";

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ObjectDefinitionsHash = "";

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ObjectDefinitionsTextHash = "";

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString ScriptFragmentsHash = "";

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString HierarchyHash = "";

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ScriptMethodsHash = "";

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonRoot);

	bool DidObjectDefsOrGVsChange() const { return bObjectDefsOrGVsChanged; }
	bool DidScriptFragmentsChange() const { return bScriptFragmentsChanged; }

	void SetObjectDefinitionsRebuilt() { bObjectDefsOrGVsChanged = false; }
	void SetScriptFragmentsRebuilt() { bScriptFragmentsChanged = false; }

	void SetObjectDefinitionsNeedRebuild() { bObjectDefsOrGVsChanged = true; }
	void SetScriptFragmentsNeedRebuild() { bScriptFragmentsChanged = true; }


protected:
	//unused in the UE plugin
	UPROPERTY(VisibleAnywhere, Category="Settings", meta=(DisplayName="set_Localization - unused in UE"))
	bool set_Localization = false;

private:
	bool bObjectDefsOrGVsChanged = true;

	bool bScriptFragmentsChanged = true;
};

/**
 * The Project object in the .json file.
 */
USTRUCT()
struct FArticyProjectDef
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Project")
	FString Name;
	UPROPERTY(VisibleAnywhere, Category="Project")
	FString DetailName;
	UPROPERTY(VisibleAnywhere, Category="Project")
	FString Guid;
	UPROPERTY(VisibleAnywhere, Category="Project")
	FString TechnicalName;

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonRoot);
};

UENUM()
enum class EArticyType : uint8
{
	ADT_Boolean,
	ADT_Integer,
	ADT_String,
	ADT_MultiLanguageString
};

USTRUCT()
struct FArticyGVar
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Variable")
	FString Variable;
	UPROPERTY(VisibleAnywhere, Category="Variable")
	EArticyType Type = EArticyType::ADT_String;
	UPROPERTY(VisibleAnywhere, Category="Variable")
	FString Description;

	UPROPERTY(VisibleAnywhere, Category = "Variable")
	bool BoolValue = false;
	UPROPERTY(VisibleAnywhere, Category = "Variable")
	int IntValue = 0;
	UPROPERTY(VisibleAnywhere, Category = "Variable")
	FString StringValue;

	/** Returns the UArticyVariable type to be used for this variable. */
	FString GetCPPTypeString() const;
	FString GetCPPValueString() const;

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonVar);
};

USTRUCT()
struct FArticyGVNamespace
{
	GENERATED_BODY()

public:
	/** The name of this namespace */
	UPROPERTY(VisibleAnywhere, Category="Namespace")
	FString Namespace;
	UPROPERTY(VisibleAnywhere, Category="Namespace")
	FString Description;
	UPROPERTY(VisibleAnywhere, Category="Namespace")
	TArray<FArticyGVar> Variables;

	UPROPERTY(VisibleAnywhere, Category="Namespace")
	FString CppTypename;

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonNamespace, const UArticyImportData* Data);
};

USTRUCT()
struct FArticyGVInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="GlobalVariables")
	TArray<FArticyGVNamespace> Namespaces;

	void ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json, const UArticyImportData* Data);
};

//---------------------------------------------------------------------------//

USTRUCT()
struct FAIDScriptMethodParameter
{
	GENERATED_BODY()

	FAIDScriptMethodParameter() {}
	FAIDScriptMethodParameter(FString InType, FString InName) : Type(InType), Name(InName) {}
	
	UPROPERTY(VisibleAnywhere, Category = "ScriptMethods")
	FString Type;

	UPROPERTY(VisibleAnywhere, Category = "ScriptMethods")
	FString Name;
};

USTRUCT()
struct FAIDScriptMethod
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString Name;
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString BlueprintName;
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	bool bIsOverloadedFunction = false;

	/** A list of parameters (type + parameter name), to be used in a method declaration. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FAIDScriptMethodParameter> ParameterList;
	/** A list of arguments (values), including a leading comma, to be used when calling a method. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FString> ArgumentList;
	/** A list of parameters (original types), used for generating the blueprint function display name. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FString> OriginalParameterTypes;

	const FString& GetCPPReturnType() const;
	const FString& GetCPPDefaultReturn() const;
	const FString GetCPPParameters() const;
	const FString GetArguments() const;
	const FString GetOriginalParametersForDisplayName() const;

	void ImportFromJson(TSharedPtr<FJsonObject> Json, TSet<FString> &OverloadedMethods);

private:
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString ReturnType;
};

USTRUCT()
struct FAIDUserMethods
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FAIDScriptMethod> ScriptMethods;

	void ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json);
};

/**
 * A single language definition
 */
USTRUCT()
struct FArticyLanguageDef
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Language")
	FString CultureName;
	UPROPERTY(VisibleAnywhere, Category="Language")
	FString ArticyLanguageId;
	UPROPERTY(VisibleAnywhere, Category="Language")
	FString LanguageName;
	UPROPERTY(VisibleAnywhere, Category="Language")
	bool IsVoiceOver;

	void ImportFromJson(const TSharedPtr<FJsonObject>& JsonRoot);
};

/**
 * The Languages object in the manifest file.
 */
USTRUCT()
struct FArticyLanguages
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Languages")
	TMap<FString, FArticyLanguageDef> Languages;

	void ImportFromJson(const TSharedPtr<FJsonObject>& JsonRoot);
};

/*Used as a workaround to store an array in a map*/
USTRUCT()
struct FArticyIdArray
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FArticyId> Values;
};

//---------------------------------------------------------------------------//

UCLASS(BlueprintType)
class UADIHierarchyObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	FString Id;
	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	FString TechnicalName;
	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	FString Type;

	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	TArray<UADIHierarchyObject*> Children;

	static UADIHierarchyObject* CreateFromJson(UObject* Outer, const TSharedPtr<FJsonObject> JsonObject);
};

USTRUCT()
struct FADIHierarchy
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Hierarchy")
	UADIHierarchyObject* RootObject = nullptr;

	void ImportFromJson(UArticyImportData* ImportData, const TSharedPtr<FJsonObject> JsonRoot);
};

USTRUCT()
struct FArticyExpressoFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Script")
	FString OriginalFragment = "";
	UPROPERTY(VisibleAnywhere, Category = "Script")
	FString ParsedFragment = "";
	UPROPERTY(VisibleAnywhere, Category = "Script")
	bool bIsInstruction = false;

	bool operator==(const FArticyExpressoFragment& Other) const
	{
		return bIsInstruction == Other.bIsInstruction && OriginalFragment == Other.OriginalFragment;
	}
};

inline uint32 GetTypeHash(const FArticyExpressoFragment& A)
{
	return GetTypeHash(A.OriginalFragment) ^ GetTypeHash(A.bIsInstruction);
}

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
		FArticyLanguages Languages;

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

	void ImportFromJson(const UArticyArchiveReader& Archive, const TSharedPtr<FJsonObject> RootObject);

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
	FArticyLanguages Languages;

	UPROPERTY(VisibleAnywhere, Category="ImportData")
	TSet<FArticyExpressoFragment> ScriptFragments;

	UPROPERTY(VisibleAnywhere, Category = "Imported")
	TArray<TSoftObjectPtr<UArticyPackage>> ImportedPackages;

	UPROPERTY(VisibleAnywhere, Category="Imported")
	TMap<FArticyId, FArticyIdArray> ParentChildrenCache;
};

