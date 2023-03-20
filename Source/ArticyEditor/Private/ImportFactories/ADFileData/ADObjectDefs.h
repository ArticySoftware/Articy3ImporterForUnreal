//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ADObjectDefs.generated.h"

USTRUCT()
struct FArticyTemplateConstraint
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="TemplateConstraint")
	FString Property = "";
	UPROPERTY(VisibleAnywhere, Category="TemplateConstraint")
	FString Type = "";
	/** Only present on localizable properties. */
	UPROPERTY(VisibleAnywhere, Category="TemplateConstraint")
	bool IsLocalized = false;
};

USTRUCT()
struct FArticyPropertyDef
{
	GENERATED_BODY()
	
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
};

USTRUCT()
struct FArticyEnumValue
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="EnumValue")
	FString Name = "";
	UPROPERTY(VisibleAnywhere, Category="EnumValue")
	uint8 Value = 0; //0..255, limited by UE!
};

USTRUCT()
struct FArticyTemplateFeatureDef
{
	GENERATED_BODY()

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

	UPROPERTY(VisibleAnywhere, Category="Template")
	FString TechnicalName = "";
	UPROPERTY(VisibleAnywhere, Category="Template")
	FString DisplayName = "";

	UPROPERTY(VisibleAnywhere, Category="Template")
	TArray<FArticyTemplateFeatureDef> Features;
};

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
struct ARTICYEDITOR_API FADObjectDefs
{
	GENERATED_BODY()

	/**
	 * This stores information about all the types that have been imported.
	 * Note that FName is case-insensitive, as is the type information in the
	 * articy json export.
	 */
	UPROPERTY(VisibleAnywhere, Category="ObjectDefinitions")
	TMap<FName, FArticyObjectDef> Types;

	UPROPERTY(VisibleAnywhere, Category = "ObjectDefinitions")
	TMap<FName, FArticyTemplateFeatureDef> FeatureDefs;	
};
