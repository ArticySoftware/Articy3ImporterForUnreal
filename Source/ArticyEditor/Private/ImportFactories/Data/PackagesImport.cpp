//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "PackagesImport.h"
#include "ArticyEditorModule.h"
#include "ArticyImporterHelpers.h"
#include "ImportFactories/Data/ArticyImportData.h"
#include "ArticyObject.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/Paths.h"
#include "Misc/App.h"
#include "UObject/ConstructorHelpers.h"

#include "ArticyPluginSettings.h"
#include "FileHelpers.h"

#define STRINGIFY(x) #x

void FArticyModelDef::ImportFromJson(const TSharedPtr<FJsonObject> JsonModel)
{
	JSON_TRY_FNAME(JsonModel, Type);
	JSON_TRY_STRING(JsonModel, AssetRef);

	FString Category;
	JSON_TRY_STRING(JsonModel, Category);
	this->AssetCategory = GetAssetCategoryFromString(Category);

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

UArticyObject* FArticyModelDef::GenerateSubAsset(const UArticyImportData* Data, UObject* Outer) const
{
	//the class is found by taking the CPP name and removing the first character
	auto className = Data->GetObjectDefs().GetCppType(Type, Data, false);
	className.RemoveAt(0);

	//generate the asset

	auto fullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), FApp::GetProjectName(), *className);
	auto uclass = ConstructorHelpersInternal::FindOrLoadClass(fullClassName, UArticyObject::StaticClass());
	if (uclass)
	{
		auto obj = ArticyImporterHelpers::GenerateSubAsset<UArticyObject>(*className, FApp::GetProjectName(), GetNameAndId(), Outer);
		FAssetRegistryModule::AssetCreated(Cast<UObject>(obj));
		if (ensure(obj))
		{
			obj->Initialize();
			Data->GetObjectDefs().InitializeModel(obj, *this, Data);

			//SAVE!!

			//Packing->AddAsset(Package, obj);
			obj->MarkPackageDirty();
		}

		return obj;
	}
	return nullptr;
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

EArticyAssetCategory FArticyModelDef::GetAssetCategoryFromString(const FString Category)
{
	if (Category == "Image") return EArticyAssetCategory::Image;
	else if (Category == "Video") return EArticyAssetCategory::Video;
	else if (Category == "Audio") return EArticyAssetCategory::Audio;
	else if (Category == "Document") return EArticyAssetCategory::Document;
	else if (Category == "Misc") return EArticyAssetCategory::Misc;
	else if (Category == "All") return EArticyAssetCategory::All;
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

UArticyPackage* FArticyPackageDef::GeneratePackageAsset(UArticyImportData* Data) const
{
	const FString PackageName = GetFolder();
	const FString PackagePath = ArticyHelpers::GetArticyGeneratedFolder() / PackageName;

	// @TODO Engine Versioning
#if ENGINE_MAJOR_VERSION >= 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 26)
	UPackage* AssetPackage = CreatePackage(*PackagePath);
#else
	UPackage* AssetPackage = CreatePackage(nullptr, *PackagePath);
#endif

	AssetPackage->FullyLoad();

	const FString AssetName = FPaths::GetBaseFilename(PackageName);

	UArticyPackage* ArticyPackage = ArticyImporterHelpers::GenerateAsset<UArticyPackage>(*UArticyPackage::StaticClass()->GetName(), TEXT("ArticyRuntime"), *AssetName, "Packages");

	ArticyPackage->Clear();
	ArticyPackage->Name = Name;
	ArticyPackage->Description = Description;
	ArticyPackage->bIsDefaultPackage = IsDefaultPackage;

	// create all contained subassets and register them in the package
	for (const auto model : Models)
	{
		UArticyObject* asset = model.GenerateSubAsset(Data, ArticyPackage); //MM_CHANGE

		if (asset)
		{
			FString id = ArticyHelpers::Uint64ToHex(asset->GetId());
			ArticyPackage->AddAsset(asset);
			Data->AddChildToParentCache(model.GetParent(), model.GetId());
		}
	}
	
	FAssetRegistryModule::AssetCreated(ArticyPackage);

	// AssetPackage->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(ArticyPackage);
	AssetPackage->SetDirtyFlag(true);
	
	if (!UEditorLoadingAndSavingUtils::SavePackages({AssetPackage}, true))
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Failed to save packages. Make sure to save before submitting in Perforce."));
	}
	
	return ArticyPackage;	
}

FString FArticyPackageDef::GetFolder() const
{
	return (FString(TEXT("Packages")) / Name).Replace(TEXT(" "), TEXT("_"));
}

FString FArticyPackageDef::GetFolderName() const
{
	int32 pathCutOffIndex = INDEX_NONE;
	FString folder = this->GetFolder();
	folder.FindLastChar('/', pathCutOffIndex);

	if (pathCutOffIndex != INDEX_NONE)
	{
		return this->GetFolder().RightChop(pathCutOffIndex);
	}
	else
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Could not retrieve folder name for package %s! Did GetFolder() method change?"), *this->Name);
		return FString(TEXT("Invalid"));
	}
}

const FString FArticyPackageDef::GetName() const
{
	return Name;
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
	auto& ArticyPackages = Data->GetPackages();

	ArticyPackages.Reset(Packages.Num());
	
	for (auto pack : Packages)
	{
		ArticyPackages.Add(pack.GeneratePackageAsset(Data));
	}

	//store gathered information about who has which children in generated assets
	auto parentChildrenCache = Data->GetParentChildrenCache();
	const auto childrenProp = FName{ TEXT("Children") };
	for (auto pack : ArticyPackages)
	{
		for (auto obj : pack->GetAssets())
		{
			if (auto articyObj = Cast<UArticyObject>(obj))
			{
				if (auto children = parentChildrenCache.Find(articyObj->GetId()))
				{
					// if the setting is enabled, try to sort. Will only work with exported position properties.
					if(GetDefault<UArticyPluginSettings>()->bSortChildrenAtGeneration)
					{
						children->Values.Sort(ArticyImporterHelpers::FCompareArticyNodeXLocation());
					}
					articyObj->SetProp(childrenProp, children->Values);
				}
			}
		}
	}
}

TSet<FString> FArticyPackageDefs::GetPackageNames() const
{
	TSet<FString> outArray;
	for(FArticyPackageDef def : Packages)
	{
		outArray.Add(def.GetName());
	}

	return outArray;
}
