//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "ArticyJSONFactory.h"
#include "PredefinedTypes.h"
#include "ArticyObject.h"

#include "ObjectDefinitionsImport.generated.h"

class UArticyObject;
struct FArticyModelDef;
class CodeFileGenerator;
struct FArticyObjectDefinitions;

USTRUCT()
struct FArticyTemplateConstraint
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category="TemplateConstraint")
	FString Property = "";
	UPROPERTY(VisibleAnywhere, Category="TemplateConstraint")
	FString Type = "";

	/** Only present on localizable properties. */
	UPROPERTY(VisibleAnywhere, Category="TemplateConstraint")
	bool IsLocalized = false;

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonProperty);
};

USTRUCT()
struct FArticyPropertyDef
{
	GENERATED_BODY()
	
public:
	void ImportFromJson(const TSharedPtr<FJsonObject> JsonProperty, const UArticyImportData* Data, const TArray<FArticyTemplateConstraint>* OptionalConstraints = nullptr);
	void GenerateCode(CodeFileGenerator& header, const UArticyImportData* Data) const;

	void GatherScript(const TSharedPtr<FJsonObject>& JsonValue, UArticyImportData* Data) const;
	void InitializeModel(UArticyBaseObject* Model, const FString& Path, const TSharedPtr<FJsonObject>& JsonValue, const UArticyImportData* Data) const;

	const FName& GetPropetyName() const { return Property; }
	const FName& GetOriginalType() const { return Type; }
	const FName& GetOriginalItemType() const { return ItemType; }
	
	FString GetCppType(const UArticyImportData* Data) const;

private:
	UPROPERTY(VisibleAnywhere, Category="ObjectProperty")
	FName Property = "";
	UPROPERTY(VisibleAnywhere, Category="ObjectProperty")
	FName Type = "";

	UPROPERTY(VisibleAnywhere, Category="ObjectProperty")
	FName ItemType = "";
	UPROPERTY(VisibleAnywhere, Category="ObjectProperty")
	FString DisplayName = "";
	UPROPERTY(VisibleAnywhere, Category="ObjectProperty")
	FString Tooltip = "";

	friend class UArticyImportData;
};

USTRUCT()
struct FArticyEnumValue
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category="EnumValue")
	FString Name = "";
	UPROPERTY(VisibleAnywhere, Category="EnumValue")
	uint8 Value = 0; //0..255, limited by UE!

	void ImportFromJson(const TPair<FString, TSharedPtr<FJsonValue>> JsonKeyValue);
};

USTRUCT()
struct FArticyTemplateFeatureDef
{
	GENERATED_BODY()

public:
	void ImportFromJson(const TSharedPtr<FJsonObject> JsonObject, const UArticyImportData* Data);
	/** Generates the USTRUCT. */
	void GenerateDefCode(CodeFileGenerator& header, const UArticyImportData* Data) const;
	/** Generates the UPROPERTY (variable). */
	void GeneratePropertyCode(CodeFileGenerator& header, const UArticyImportData* Data) const;

	void GatherScripts(const TSharedPtr<FJsonObject>& Json, UArticyImportData* Data) const;
	void InitializeModel(UArticyPrimitive* Model, const FString& Path, const TSharedPtr<FJsonObject>& Json, const UArticyImportData* Data) const;

	/** Returns the CPP type of this feature (for variable declarations if bAsVariable is true). */
	FString GetCppType(const UArticyImportData* Data, bool bAsVariable) const;
	/** Returns the UClass of this Feature. */
	UClass* GetUClass(const UArticyImportData* Data) const;

	FString GetTechnicalName() const { return TechnicalName; }
	FString GetDisplayName() const { return DisplayName; }

private:
	UPROPERTY(VisibleAnywhere, Category="TemplateFeature")
	FString TechnicalName = "";
	UPROPERTY(VisibleAnywhere, Category="TemplateFeature")
	FString DisplayName = "";

	UPROPERTY(VisibleAnywhere, Category="TemplateFeature")
	TArray<FArticyPropertyDef> Properties;

	UPROPERTY(VisibleAnywhere, Category="TemplateFeature")
	TArray<FArticyTemplateConstraint> Constraints;
};

USTRUCT()
struct FArticyTemplateDef
{
	GENERATED_BODY()
	
public:
	
	void ImportFromJson(const TSharedPtr<FJsonObject> JsonObject, const UArticyImportData* Data);
	void GenerateFeaturesDefs(CodeFileGenerator& header, const UArticyImportData* Data) const;
	void GenerateProperties(CodeFileGenerator& header, const UArticyImportData* Data) const;

	void GatherScripts(const TSharedPtr<FJsonObject> Values, UArticyImportData* Data) const;
	void InitializeModel(UArticyPrimitive* Model, const FString& Path, const TSharedPtr<FJsonObject> Values, const UArticyImportData* Data) const;

	FString GetDisplayName() const { return DisplayName; }
	const TArray<FArticyTemplateFeatureDef>& GetFeatures() const { return Features; }

private:

	UPROPERTY(VisibleAnywhere, Category="Template")
	FString TechnicalName = "";
	UPROPERTY(VisibleAnywhere, Category="Template")
	FString DisplayName = "";

	UPROPERTY(VisibleAnywhere, Category="Template")
	TArray<FArticyTemplateFeatureDef> Features;
};

//---------------------------------------------------------------------------//

UENUM()
enum class EObjectDefType : uint8
{
	Model, //just a Type with properties, no Template
	Enum,
	Template,
};

USTRUCT()
struct FArticyObjectDef
{
	GENERATED_BODY()

public:

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonObjDef, const UArticyImportData* Data);

	/** Returns true if the given property is already defined in the base class. */
	bool IsBaseProperty(FName Property, const UArticyImportData* Data) const;
	void GenerateCode(CodeFileGenerator& header, const UArticyImportData* Data) const;

	/** Find all script fragments, add them to the UArticyImportData, and replace them with an id. */
	void GatherScripts(const FArticyModelDef& Values, UArticyImportData* Data) const;
	void InitializeModel(UArticyPrimitive* Model, const FArticyModelDef& Values, const UArticyImportData* Data) const;

	FString GetCppType(const UArticyImportData* Data, const bool bForProperty) const;
	FString GetCppBaseClasses(const UArticyImportData* Data) const;
	const FName& GetOriginalType() const { return Type; }
	const TArray<FArticyTemplateFeatureDef>& GetFeatures() const;

private:

	UPROPERTY(VisibleAnywhere, Category="ObjectDef")
	EObjectDefType DefType = EObjectDefType::Enum;

	UPROPERTY(VisibleAnywhere, Category="ObjectDef")
	FName Type = "";
	UPROPERTY(VisibleAnywhere, Category="ObjectDef")
	FName Class = "";
	UPROPERTY(VisibleAnywhere, Category="ObjectDef")
	FName InheritsFrom = "";

	/** Properties of the model. */
	UPROPERTY(VisibleAnywhere, Category="ObjectDef")
	TArray<FArticyPropertyDef> Properties;
	/** Might be empty. */
	UPROPERTY(VisibleAnywhere, Category="ObjectDef")
	FArticyTemplateDef Template;

	/** Only for enums. */
	UPROPERTY(VisibleAnywhere, Category="ObjectDef")
	TArray<FArticyEnumValue> Values;
};

USTRUCT()
struct FArticyObjectDefinitions
{
	GENERATED_BODY()

public:
	
	void ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json, const UArticyImportData* Data);
	void GatherScripts(const FArticyModelDef& Values, UArticyImportData* Data) const;
	void InitializeModel(UArticyPrimitive* Model, const FArticyModelDef& Values, const UArticyImportData* Data) const;

	FString GetCppType(const FName& OriginalType, const UArticyImportData* Data, const bool bForProperty) const;
	static const FString& GetCppDefaultValue(const FName& OriginalType);

	struct FClassInfo
	{
		FClassInfo() = default;
		FClassInfo(const FString& Name, UClass* Class) : CppTypeName(Name), StaticClass(Class) {}

		FString CppTypeName;
		UClass* StaticClass;
	};

	static const FClassInfo& GetDefaultBaseClass(const FName& OriginalType, const UArticyImportData* Data);

	/**
	 * Returns the interface name that contains a getter for the specified property,
	 * or an empty name if there is no such interface.
	 */
	static const FName& GetProviderInterface(const FArticyPropertyDef& Property);

	static void SetProp(const FName& OriginalType, const FName& Property, PROP_SETTER_PARAMS);

	bool IsNewFeatureType(const FName& CppType) const;
	TMap<FName, FArticyObjectDef>& GetTypes() { return Types; }
	const TMap<FName, FArticyObjectDef>& GetTypes() const { return Types; }
	const TMap<FName, FArticyTemplateFeatureDef> GetFeatures() const { return FeatureDefs; }

private:

	/**
	 * This stores information about all the types that have been imported.
	 * Note that FName is case-insensitive, as is the type information in the
	 * articy json export.
	 */
	UPROPERTY(VisibleAnywhere, Category="ObjectDefinitions")
	TMap<FName, FArticyObjectDef> Types;

	/**
	 * Contains the CPP type of all defined features.
	 * This is needed to prevent feature types from being imported multiple times (as they are defined multiple times in the json).
	 */
	UPROPERTY(VisibleAnywhere, Category="ObjectDefinitions")
	mutable TSet<FName> FeatureTypes;

	UPROPERTY(VisibleAnywhere, Category = "ObjectDefinitions")
	TMap<FName, FArticyTemplateFeatureDef> FeatureDefs;
};
