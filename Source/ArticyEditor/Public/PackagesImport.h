//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyAsset.h"
#include "ArticyBaseTypes.h"
#include "ArticyPackage.h"


#include "PackagesImport.generated.h"


class UArticyImportData;

USTRUCT(BlueprintType)
struct FArticyModelDef
{
	GENERATED_BODY()

public:
	
	void ImportFromJson(const TSharedPtr<FJsonObject> JsonModel);
	void GatherScripts(UArticyImportData* Data) const;
	UArticyObject* GenerateSubAsset(const UArticyImportData* Data, UObject* Outer) const;//MM_CHANGE


	FName GetType() const { return Type; }
	const FString& GetTechnicalName() const { return TechnicalName; }
	const FString& GetNameAndId() const { return NameAndId; }
	const FArticyId& GetId() const { return Id; }
	const FArticyId& GetParent() const { return Parent; }

	const FString& GetAssetRef() const { return AssetRef; }
	const EArticyAssetCategory& GetAssetCat() const { return AssetCategory; }
	TSharedPtr<FJsonObject> GetPropertiesJson() const;
	TSharedPtr<FJsonObject> GetTemplatesJson() const;

private:

	EArticyAssetCategory GetAssetCategoryFromString(const FString Category);

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
	EArticyAssetCategory AssetCategory = EArticyAssetCategory::None;

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
	UArticyPackage* GeneratePackageAsset(UArticyImportData* Data) const;//MM_CHANGE


	FString GetFolder() const;
	FString GetFolderName() const;
	const FString GetName() const;

private:

	UPROPERTY(VisibleAnywhere, Category = "Package")
	FString Name;

	// ArticyX ---------------
	UPROPERTY(VisibleAnywhere, Category = "Package")
	FString Id;
	
	UPROPERTY(VisibleAnywhere, Category = "Package")
	FString Description;
	UPROPERTY(VisibleAnywhere, Category = "Package")
	bool IsDefaultPackage = false;

	// ArticyX ---------------
	UPROPERTY(VisibleAnywhere, Category = "Package")
	bool IsIncluded = false;

	UPROPERTY(VisibleAnywhere, Category = "Package")
	TArray<FADIFileHash> PackagesHashes;
	
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
	void GenerateAssets(UArticyImportData* Data) const;//MM_CHANGE

	TSet<FString> GetPackageNames() const;
private:

	UPROPERTY(VisibleAnywhere, Category = "Packages")
	TArray<FArticyPackageDef> Packages;
};
