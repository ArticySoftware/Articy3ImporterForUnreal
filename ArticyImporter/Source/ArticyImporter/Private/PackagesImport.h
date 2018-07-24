//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyAsset.h"
#include "ArticyBaseTypes.h"

#include "PackagesImport.generated.h"


class UArticyImportData;

USTRUCT(BlueprintType)
struct FArticyModelDef
{
	GENERATED_BODY()

public:
	
	void ImportFromJson(const TSharedPtr<FJsonObject> JsonModel);
	void GatherScripts(UArticyImportData* Data) const;
	UArticyObject* GenerateAsset(const UArticyImportData* Data, const FString Package) const;

	FName GetType() const { return Type; }
	const FString& GetTechnicalName() const { return TechnicalName; }
	const FString& GetNameAndId() const { return NameAndId; }
	const FArticyId& GetId() const { return Id; }
	const FArticyId& GetParent() const { return Parent; }

	const FString& GetAssetRef() const { return AssetRef; }
	const EArticyAssetCategory& GetAssetCat() const { return Category; }
	TSharedPtr<FJsonObject> GetPropertiesJson() const;
	TSharedPtr<FJsonObject> GetTemplatesJson() const;

private:

	EArticyAssetCategory GetAssetCategoryFromString(const FString AssetCategory);

	/** The original type of the model as read in from json. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FName Type;

	/** The TechnicalName of the model, extracted from the PropertiesJsonString. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString TechnicalName;
	/** The Id of the model, extracted from the PropertiesJsonString. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FArticyId Id = -1;

	/** TechnicalName(Id) */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString NameAndId;

	/** The Id of the parent of this model, extracted from the PropertiesJsonString. */
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FArticyId Parent = 0;

	/** The asset reference, if this is an asset. */
	UPROPERTY(VisibleAnywhere, Category = "Model Meta")
	FString AssetRef;
	/** The asset category, if this is an asset. */
	UPROPERTY(VisibleAnywhere, Category = "Model Meta")
	EArticyAssetCategory Category = EArticyAssetCategory::None;

	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString PropertiesJsonString;
	UPROPERTY(VisibleAnywhere, Category = "Model")
	FString TemplateJsonString;

	mutable TSharedPtr<FJsonObject> CachedPropertiesJson = nullptr;
	mutable TSharedPtr<FJsonObject> CachedTemplateJson = nullptr;
};

USTRUCT(BlueprintType)
struct FArticyPackageDef
{
	GENERATED_BODY()

public:

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonPackage);
	void GatherScripts(UArticyImportData* Data) const;
	FArticyPackage GenerateAssets(UArticyImportData* Data) const;

	FString GetFolder() const;

private:

	UPROPERTY(VisibleAnywhere, Category = "Package")
	FString Name;
	UPROPERTY(VisibleAnywhere, Category = "Package")
	FString Description;
	UPROPERTY(VisibleAnywhere, Category = "Package")
	bool IsDefaultPackage = false;

	UPROPERTY(VisibleAnywhere, Category = "Package")
	TArray<FArticyModelDef> Models;
};

/** Contains information about all imported packages. */
USTRUCT(BlueprintType)
struct FArticyPackageDefs
{
	GENERATED_BODY()

public:
	
	void ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json);
	void GatherScripts(UArticyImportData* Data) const;
	void GenerateAssets(UArticyImportData* Data) const;

private:

	UPROPERTY(VisibleAnywhere, Category = "Packages")
	TArray<FArticyPackageDef> Packages;
};
