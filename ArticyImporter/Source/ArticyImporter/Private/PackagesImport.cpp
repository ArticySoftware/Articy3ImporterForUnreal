//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyImporterPrivatePCH.h"

#include "PackagesImport.h"
#include "ArticyImportData.h"
#include "CodeGeneration/CodeGenerator.h"
#include "ArticyObject.h"
#include <string>

#define STRINGIFY(x) #x

void FArticyModelDef::ImportFromJson(const TSharedPtr<FJsonObject> JsonModel)
{
	JSON_TRY_FNAME(JsonModel, Type);
	JSON_TRY_STRING(JsonModel, AssetRef);

	FString Category;
	JSON_TRY_STRING(JsonModel, Category);
	this->Category = GetAssetCategoryFromString(Category);

	{
		PropertiesJsonString = "";
		TSharedPtr<FJsonObject> Properties = JsonModel->GetObjectField(TEXT("Properties"));
		if(Properties.IsValid())
		{
			JSON_TRY_STRING(Properties, TechnicalName);
			JSON_TRY_HEX_ID(Properties, Id);
			JSON_TRY_HEX_ID(Properties, Parent);

			FString stringId;
			Properties->TryGetStringField(TEXT("Id"), stringId);
			NameAndId = FString::Printf(TEXT("%s_%s"), *TechnicalName, *stringId);

			//serialize the Properties to string, using the condensed writer to save memory
			TSharedRef< TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&PropertiesJsonString);
			FJsonSerializer::Serialize(Properties.ToSharedRef(), Writer);
		}
	}

	{
		TemplateJsonString = "";
		const TSharedPtr<FJsonObject>* Template;
		if(JsonModel->TryGetObjectField(TEXT("Template"), Template))
		{
			TSharedRef< TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&TemplateJsonString);
			FJsonSerializer::Serialize(Template->ToSharedRef(), Writer);
		}
	}
}

void FArticyModelDef::GatherScripts(UArticyImportData* Data) const
{
	Data->GetObjectDefs().GatherScripts(*this, Data);
}

UArticyObject* FArticyModelDef::GenerateAsset(const UArticyImportData* Data, const FString Package) const
{
	//the class is found by taking the CPP name and removing the first character
	auto className = Data->GetObjectDefs().GetCppType(Type, Data, false);
	className.RemoveAt(0);

	//generate the asset
	auto obj = ArticyHelpers::GenerateAsset<UArticyObject>(*className, FApp::GetProjectName(), GetNameAndId(), Package);
	if(ensure(obj))
	{
		obj->Initialize();
		Data->GetObjectDefs().InitializeModel(obj, *this, Data);

		//SAVE!!
		obj->MarkPackageDirty();
	}

	return obj;
}

TSharedPtr<FJsonObject> FArticyModelDef::GetPropertiesJson() const
{
	if(!CachedPropertiesJson.IsValid())
	{
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(PropertiesJsonString);
		FJsonSerializer::Deserialize(JsonReader, CachedPropertiesJson);

		if(!CachedPropertiesJson.IsValid())
			CachedPropertiesJson = MakeShareable(new FJsonObject);
	}

	return CachedPropertiesJson;
}

TSharedPtr<FJsonObject> FArticyModelDef::GetTemplatesJson() const
{
	if(!CachedTemplateJson.IsValid())
	{
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(TemplateJsonString);
		FJsonSerializer::Deserialize(JsonReader, CachedTemplateJson);

		if(!CachedTemplateJson.IsValid())
			CachedTemplateJson = MakeShareable(new FJsonObject);
	}

	return CachedTemplateJson;
}

EArticyAssetCategory FArticyModelDef::GetAssetCategoryFromString(const FString AssetCategory)
{
	if (AssetCategory == "Image") return EArticyAssetCategory::Image;
	else if (AssetCategory == "Video") return EArticyAssetCategory::Video;
	else if (AssetCategory == "Audio") return EArticyAssetCategory::Audio;
	else if (AssetCategory == "Document") return EArticyAssetCategory::Document;
	else if (AssetCategory == "Misc") return EArticyAssetCategory::Misc;
	else if (AssetCategory == "All") return EArticyAssetCategory::All;
	else return EArticyAssetCategory::None;
}

//---------------------------------------------------------------------------//

void FArticyPackageDef::ImportFromJson(const TSharedPtr<FJsonObject> JsonPackage)
{
	Models.Reset();

	if(!JsonPackage.IsValid())
		return;

	JSON_TRY_STRING(JsonPackage, Name);
	JSON_TRY_STRING(JsonPackage, Description);
	JSON_TRY_BOOL(JsonPackage, IsDefaultPackage);

	JSON_TRY_ARRAY(JsonPackage, Models,
	{
		auto obj = item->AsObject();
		if(obj.IsValid())
		{
			FArticyModelDef model;
			model.ImportFromJson(obj);
			Models.Add(model);
		}
	});
}

void FArticyPackageDef::GatherScripts(UArticyImportData* Data) const
{
	for(const auto& model : Models)
		Data->GetObjectDefs().GatherScripts(model, Data);
}

FArticyPackage FArticyPackageDef::GenerateAssets(UArticyImportData* Data) const
{
	FArticyPackage package;
	package.Name = Name;
	package.Description = Description;
	package.bIsDefaultPackage = IsDefaultPackage;

	for(const auto model : Models)
	{
		auto asset = model.GenerateAsset(Data, GetFolder());
		if (asset)
		{
			package.Objects.Add(asset);
			Data->AddChildToParentCache(model.GetParent(), model.GetId());
		}
	}

	return package;
}

FString FArticyPackageDef::GetFolder() const
{
	return (FString(TEXT("Packages")) / Name).Replace(TEXT(" "), TEXT("_"));
}

//---------------------------------------------------------------------------//

void FArticyPackageDefs::ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json)
{
	Packages.Reset();

	if(!Json)
		return;

	for(const auto pack : *Json)
	{
		const auto obj = pack->AsObject();
		if(!obj.IsValid())
			continue;

		FArticyPackageDef package;
		package.ImportFromJson(obj);
		Packages.Add(package);
	}
}

void FArticyPackageDefs::GatherScripts(UArticyImportData* Data) const
{
	for(const auto& pack : Packages)
		pack.GatherScripts(Data);
}

void FArticyPackageDefs::GenerateAssets(UArticyImportData* Data) const
{
	auto& importedPackages = Data->GetPackages();
	importedPackages.Reset(Packages.Num());

	for(auto pack : Packages)
		importedPackages.Add(pack.GenerateAssets(Data));

	//store gathered information about who has which children in generated assets
	auto parentChildrenCache = Data->GetParentChildrenCache();
	const auto childrenProp = FName{ TEXT("Children") };
	for (auto pack : importedPackages)
	{
		for (auto obj : pack.Objects)
		{
			if (auto articyObj = Cast<UArticyObject>(obj.Get()))
			{
				if (auto children = parentChildrenCache.Find(articyObj->GetId()))
				{
					articyObj->SetProp(childrenProp, children->Values);
				}
			}
		}
	}
}
