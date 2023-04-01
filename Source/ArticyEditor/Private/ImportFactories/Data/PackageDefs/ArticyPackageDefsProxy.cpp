#include "ArticyPackageDefsProxy.h"

#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"

FArticyPackageDefs ArticyPackageDefsProxy::fromJson(const TArray<TSharedPtr<FJsonValue>>* Json)
{
	_packageDefs.Packages.Reset();

	if (!Json)
		return {};

	for (const auto pack : *Json)
	{
		const auto obj = pack->AsObject();
		if (!obj.IsValid())
			continue;

		FArticyPackageDef package;
		GetPackageDef(package, obj);
		if(!package.Name.IsEmpty())
		{
			_packageDefs.Packages.Add(package);	
		}
	}

	return _packageDefs;
}

void ArticyPackageDefsProxy::GetPackageDef(FArticyPackageDef& pDef, const TSharedPtr<FJsonObject> Json)
{
	pDef.Models.Reset();
	
	if(!Json.IsValid())
		return;

	Json->TryGetStringField(TEXT("Name"), pDef.Name);
	Json->TryGetStringField(TEXT("Description"), pDef.Description);
	Json->TryGetBoolField(TEXT("IsDefaultPackage"), pDef.IsDefaultPackage);
	
	const TArray<TSharedPtr<FJsonValue>>* items;
	Json->TryGetArrayField(TEXT("Models"), items);
	for(const auto item : *items)
	{
		auto obj = item->AsObject();
		if(obj.IsValid())
		{
			FArticyModelDef model;
			GetModelDef(model, obj);
			pDef.Models.Add(model);
		}
	};
}

void ArticyPackageDefsProxy::GetModelDef(FArticyModelDef& modelDef, const TSharedPtr<FJsonObject> Json)
{
	FString Fstype;
	Json->TryGetStringField(TEXT("Type"), Fstype);
	modelDef.Type = FName(Fstype);

	Json->TryGetStringField(TEXT("AssetRef"), modelDef.AssetRef);

	FString Category;
	Json->TryGetStringField(TEXT("Category"), Category);
	modelDef.AssetCategory = GetAssetCategoryFromString(Category);
	
	modelDef.PropertiesJsonString = "";
	TSharedPtr<FJsonObject> Properties = Json->GetObjectField(TEXT("Properties"));
	if(Properties.IsValid())
	{
		Properties->TryGetStringField(TEXT("TechnicalName"), modelDef.TechnicalName );
		Properties->TryGetStringField(TEXT("Id"), modelDef.Id );
#pragma	NOTE( ArticyPackageDefsProxy::GetModelDef : ModelDef.Id => ArticyID )

		Properties->TryGetStringField(TEXT("Parent"), modelDef.Parent );
#pragma	NOTE( ArticyPackageDefsProxy::GetModelDef : ModelDef.Parent => ArticyID )		

		FString stringId;
		Properties->TryGetStringField(TEXT("Id"), stringId);
		modelDef.NameAndId = FString::Printf(TEXT("%s_%s"), *modelDef.TechnicalName, *stringId);

		//serialize the Properties to string, using the condensed writer to save memory
		TSharedRef< TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&modelDef.PropertiesJsonString);
		FJsonSerializer::Serialize(Properties.ToSharedRef(), Writer);
	}

	modelDef.TemplateJsonString = "";
	const TSharedPtr<FJsonObject>* Template;
	if(Json->TryGetObjectField(TEXT("Template"), Template))
	{
		TSharedRef< TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&modelDef.TemplateJsonString);
		FJsonSerializer::Serialize(Template->ToSharedRef(), Writer);
	}
}

EArticyAssetCategory ArticyPackageDefsProxy::GetAssetCategoryFromString(const FString Category)
{
	if (Category == "Image") return EArticyAssetCategory::Image;
	if (Category == "Video") return EArticyAssetCategory::Video;
	if (Category == "Audio") return EArticyAssetCategory::Audio;
	if (Category == "Document") return EArticyAssetCategory::Document;
	if (Category == "Misc") return EArticyAssetCategory::Misc;
	if (Category == "All") return EArticyAssetCategory::All;
	
	return EArticyAssetCategory::None;
}

