//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyAsset.h"
#include "ArticyPackageDefs.generated.h"

// UENUM()
// enum class EArticyAssetCategory : uint8
// {
// 	None = 0,
//
// 	/** All image related asset types, also icons and svg. */
// 	Image = 1,
// 	/** All video related asset types*/
// 	Video = 2,
// 	/** All audio related asset types */
// 	Audio = 4,
// 	/** All documents, like txt and doc. */
// 	Document = 8,
// 	/** Everything else, like obj, blend, fbx etc. */
// 	Misc = 16,
//
// 	/** Defines a flag to describe all assets, used when declaring supported assets. */
// 	All = 0xFF,
// };

USTRUCT()
struct FArticyModelDef
{
	GENERATED_BODY()
	
	/** The original type of the model as read in from json. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FName Type;

	/** The TechnicalName of the model, extracted from the PropertiesJsonString. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString TechnicalName;
	
	/** The Id of the model, extracted from the PropertiesJsonString. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString Id;			// @todo : FArticyID
	// FArticyId Id = -1;

	/** TechnicalName(Id) */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString NameAndId;

	/** The Id of the parent of this model, extracted from the PropertiesJsonString. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString Parent;		// @todo : FArticyID
	// FArticyId Parent = 0;

	/** The asset reference, if this is an asset. */
	UPROPERTY(VisibleAnywhere, Category = "Model Meta")
	FString AssetRef;
	
	/** The asset category, if this is an asset. */
	UPROPERTY(VisibleAnywhere, Category = "Model Meta")
	EArticyAssetCategory AssetCategory = EArticyAssetCategory::None;

	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString PropertiesJsonString;
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString TemplateJsonString;

	TSharedPtr<FJsonObject> CachedPropertiesJson = nullptr;
	TSharedPtr<FJsonObject> CachedTemplateJson = nullptr;
};

USTRUCT()
struct FArticyPackageDef
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Package")
	FString Name;
	UPROPERTY(VisibleAnywhere, Category = "Package")
	FString Description;
	UPROPERTY(VisibleAnywhere, Category = "Package")
	bool IsDefaultPackage = false;

	UPROPERTY(VisibleAnywhere, Category = "Package")
	TArray<FArticyModelDef> Models;
};

USTRUCT()
struct ARTICYEDITOR_API FArticyPackageDefs
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Packages")
	TArray<FArticyPackageDef> Packages;
};
