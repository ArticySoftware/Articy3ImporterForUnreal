//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ADJsonFileBuilder.h"
#include "ArticyEditor/Public/ArticyEditorModule.h"
#include "Dom/JsonObject.h"
#include "ImportFactories/Data/Settings/ADISettings.h"
#include "ImportFactories/Data/ArticyImportData.h"
#include "ImportFactories/Data/Project/ArticyProjectDef.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

// Not used anymore / TODELETE 
// => static class replaced with Poco/reader/dispatcher strategy.

/*
 *	// Notes //
	Initial (1.6) Import sequence :
	-------------------------------
	Settings.ImportFromJson(RootObject->GetObjectField(JSON_SECTION_SETTINGS));
	Project.ImportFromJson(RootObject->GetObjectField(JSON_SECTION_PROJECT));
	PackageDefs.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_PACKAGES));
	Hierarchy.ImportFromJson(this, RootObject->GetObjectField(JSON_SECTION_HIERARCHY));
	UserMethods.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_SCRIPTMEETHODS));

	If Objects Definition OR GV changed
	{
		GlobalVariables.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_GLOBALVARS), this);
		ObjectDefinitions.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_OBJECTDEFS), this);
	}

	if Script Fragments Change
	{
		GatherScripts()
		Regenerate code
		(recompile...)
	}
 */

void ADJsonFileBuilder::BuildAsset(UArticyImportData& FileDataAsset, const TSharedPtr<FJsonObject> RootObject)
{
	GetSettings(FileDataAsset.Settings, RootObject->GetObjectField(JSON_SECTION_SETTINGS));
	GetProjectDefinitions(FileDataAsset.Project, RootObject->GetObjectField(JSON_SECTION_PROJECT));
	GetPackages(FileDataAsset.PackageDefs, &RootObject->GetArrayField(JSON_SECTION_PACKAGES));

	GetHierarchy(); // Looks like it imports the whole articy folder hierarchy... Necessary ???
	GetUserMethods(FileDataAsset.UserMethods,&RootObject->GetArrayField(JSON_SECTION_SCRIPTMEETHODS));
	
	// Manifest related...
	GetGlobalVariables(FileDataAsset.GlobalVariables, &RootObject->GetArrayField(JSON_SECTION_GLOBALVARS));
}

/**************************************************************
				 Main
**************************************************************/
void ADJsonFileBuilder::BuildAssetVerbose(UArticyImportData& FileDataAsset, const TSharedPtr<FJsonObject> RootObject)
{
	UE_LOG(LogArticyEditor, Log, TEXT("-------- Begin big data asset builder --------"));

	FDateTime GlobalTime = FDateTime::UtcNow();

	// quick and dirty json import perfs measures.
	// May require a delegate method for better code structure.
	FDateTime StartTime = FDateTime::UtcNow();
	GetSettings(FileDataAsset.Settings, RootObject->GetObjectField(JSON_SECTION_SETTINGS));
	float LocalTimeEllapsed = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogArticyEditor, Display, TEXT("Project settings import time : %f ms"), LocalTimeEllapsed)

	StartTime = FDateTime::UtcNow();
	GetProjectDefinitions(FileDataAsset.Project, RootObject->GetObjectField(JSON_SECTION_PROJECT));
	LocalTimeEllapsed = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogArticyEditor, Display, TEXT("Project definitions import time : %f ms"), LocalTimeEllapsed)

	StartTime = FDateTime::UtcNow();
	GetPackages(FileDataAsset.PackageDefs, &RootObject->GetArrayField(JSON_SECTION_PACKAGES));
	LocalTimeEllapsed = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogArticyEditor, Display, TEXT("Packages import time : %f ms"), LocalTimeEllapsed)

	StartTime = FDateTime::UtcNow();
	GetHierarchy(); // Looks like it imports the whole articy folder hierarchy... Necessary ???
	LocalTimeEllapsed = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogArticyEditor, Display, TEXT("Hierarchy import time : %f ms"), LocalTimeEllapsed)

	StartTime = FDateTime::UtcNow();
	GetUserMethods(FileDataAsset.UserMethods,&RootObject->GetArrayField(JSON_SECTION_SCRIPTMEETHODS));
	LocalTimeEllapsed = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogArticyEditor, Display, TEXT("User methods import time : %f ms"), LocalTimeEllapsed)

	// Manifest related...
	StartTime = FDateTime::UtcNow();
	GetGlobalVariables(FileDataAsset.GlobalVariables, &RootObject->GetArrayField(JSON_SECTION_GLOBALVARS));
	LocalTimeEllapsed = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	UE_LOG(LogArticyEditor, Display, TEXT("Global variables import time : %f ms"), LocalTimeEllapsed)
	
	UE_LOG(LogArticyEditor, Display, TEXT("------------------------"))
	float TimeElapsedInMs = (FDateTime::UtcNow() - GlobalTime).GetTotalMilliseconds();
	UE_LOG(LogArticyEditor, Display, TEXT("Total time ellasped : %f ms"), TimeElapsedInMs)
	UE_LOG(LogArticyEditor, Log, TEXT("-------- End big data asset builder --------"));
}

/**************************************************************
				 Flat data
**************************************************************/
void ADJsonFileBuilder::GetSettings(FADISettings& Settings, const TSharedPtr<FJsonObject> Json)
{
	if (!Json.IsValid())
		return;

	Json->TryGetStringField(TEXT("set_TextFormatter"), Settings.set_TextFormatter);
	Json->TryGetBoolField(TEXT("set_UseScriptSupport"), Settings.set_UseScriptSupport);
	Json->TryGetStringField(TEXT("ExportVersion"), Settings.ExportVersion);
	Json->TryGetStringField(TEXT("ObjectDefinitionsHash"), Settings.ObjectDefinitionsHash);
	Json->TryGetStringField(TEXT("ScriptFragmentsHash"), Settings.ScriptFragmentsHash);
}

void ADJsonFileBuilder::GetProjectDefinitions(FArticyProjectDef& Project, const TSharedPtr<FJsonObject> Json)
{
	if (!Json.IsValid())
		return;

	Json->TryGetStringField(TEXT("Name"), Project.Name);
	Json->TryGetStringField(TEXT("DetailName"), Project.DetailName);
	Json->TryGetStringField(TEXT("Guid"), Project.Guid);
	Json->TryGetStringField(TEXT("TechnicalName"), Project.TechnicalName);
}

/**************************************************************
 				Packages
**************************************************************/
void ADJsonFileBuilder::GetPackages(FArticyPackageDefs& PacksList, const TArray<TSharedPtr<FJsonValue>>* Json)
{
	PacksList.Packages.Reset();

	if (!Json)
		return;

	for (const auto pack : *Json)
	{
		const auto obj = pack->AsObject();
		if (!obj.IsValid())
			continue;

		FArticyPackageDef package;
		GetPackageDef(package, obj);
		if(!package.Name.IsEmpty())
		{
			PacksList.Packages.Add(package);	
		}
	}
}

void ADJsonFileBuilder::GetPackageDef(FArticyPackageDef &pDef, const TSharedPtr<FJsonObject> Json)
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

void ADJsonFileBuilder::GetModelDef(FArticyModelDef &modelDef, const TSharedPtr<FJsonObject> Json)
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
		UE_LOG(LogArticyEditor, Warning,TEXT("@Todo : ModelDef.Id => ArticyID"));

		Properties->TryGetStringField(TEXT("Parent"), modelDef.Parent );
		UE_LOG(LogArticyEditor, Warning,TEXT("@Todo : ModelDef.Parent => ArticyID"));

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

EArticyAssetCategory ADJsonFileBuilder::GetAssetCategoryFromString(const FString Category)
{
	if (Category == "Image") return EArticyAssetCategory::Image;
	if (Category == "Video") return EArticyAssetCategory::Video;
	if (Category == "Audio") return EArticyAssetCategory::Audio;
	if (Category == "Document") return EArticyAssetCategory::Document;
	if (Category == "Misc") return EArticyAssetCategory::Misc;
	if (Category == "All") return EArticyAssetCategory::All;
	
	return EArticyAssetCategory::None;
}

/**************************************************************
				User methods
**************************************************************/
void ADJsonFileBuilder::GetUserMethods(FAIDUserMethods& userMethods, const TArray<TSharedPtr<FJsonValue>>* Json)
{
	userMethods.ScriptMethods.Reset(Json ? Json->Num() : 0);

	if (!Json)
		return;

	TSet<FString> OverloadedMethods;

	for (const auto smJson : *Json)
	{
		const auto obj = smJson->AsObject();
		if (!obj.IsValid())
			continue;

		FAIDScriptMethod sm;
		GetScriptMethod(sm,obj, OverloadedMethods);
		userMethods.ScriptMethods.Add(sm);
	}

	// mark overloaded methods => Not poco friendly. move elsewhere (if necessary)
	// for (auto& scriptMethod : userMethods.ScriptMethods)
	// 	scriptMethod.bIsOverloadedFunction = OverloadedMethods.Contains(scriptMethod.Name);
}

void ADJsonFileBuilder::GetScriptMethod(FAIDScriptMethod& scriptMethod, TSharedPtr<FJsonObject> Json,
	TSet<FString>& OverloadedMethods)
{
	Json->TryGetStringField(TEXT("Name"), scriptMethod.Name );
	Json->TryGetStringField(TEXT("ReturnType"), scriptMethod.ReturnType );

	scriptMethod.BlueprintName = scriptMethod.Name + TEXT("_");
	scriptMethod.ParameterList.Empty();
	scriptMethod.OriginalParameterTypes.Empty();

	const TArray<TSharedPtr<FJsonValue>>* items;

	if (Json->TryGetArrayField(TEXT("Parameters"), items))
	{
		for (const auto item : *items)
		{
			const TSharedPtr<FJsonObject>* obj;
			if (!ensure(item->TryGetObject(obj))) continue;

			//import parameter name and type
			FString Param, Type;
			(*obj)->TryGetStringField(TEXT("Param"), Param );
			(*obj)->TryGetStringField(TEXT("Type"), Type );

			// append param types to blueprint names
			FString formattedType = Type;
			formattedType[0] = FText::FromString(Type).ToUpper().ToString()[0];
			scriptMethod.BlueprintName += formattedType;

			scriptMethod.OriginalParameterTypes.Add(Type);

			//append to parameter list
			FAIDScriptMethodParameter param;
			param.Name = Param;
			param.Type = Type,
			scriptMethod.ParameterList.Emplace(param);
			scriptMethod.ArgumentList.Add(Param);
		}
	}

	if (scriptMethod.BlueprintName.EndsWith("_"))
		scriptMethod.BlueprintName.RemoveAt(scriptMethod.BlueprintName.Len() - 1);

	// determine if this is an overloaded blueprint function
	static TMap<FString, FString> UsedBlueprintMethodsNames;
	if (UsedBlueprintMethodsNames.Contains(scriptMethod.Name))
	{
		if (UsedBlueprintMethodsNames[scriptMethod.Name] != scriptMethod.BlueprintName)
			OverloadedMethods.Add(scriptMethod.Name);
	}
	else
	{
		UsedBlueprintMethodsNames.Add(scriptMethod.Name, scriptMethod.BlueprintName);
	}
}


/**************************************************************
 				Global Namespaces/Variables
**************************************************************/
void ADJsonFileBuilder::GetGlobalVariables(FArticyGVInfo& GV, const TArray<TSharedPtr<FJsonValue>>* Json)
{
	GV.Namespaces.Reset(Json ? Json->Num() : 0);

	if (!Json)
		return;

	for (const auto nsJson : *Json)
	{
		const auto obj = nsJson->AsObject();
		if (!obj.IsValid())
			continue;

		FArticyGVNamespace ns;
		GetGVNameSpace(ns, obj);
		if (!ns.Namespace.IsEmpty())
		{
			GV.Namespaces.Add(ns);
		}
	}
}

void ADJsonFileBuilder::GetGVNameSpace(FArticyGVNamespace& GVNamespace, const TSharedPtr<FJsonObject> JsonNamespace)
{
	if (!JsonNamespace.IsValid())
		return;

	const TArray<TSharedPtr<FJsonValue>>* varsJson;
	// If the namespace don't have any value, then the Namespace field
	// Won't be filled, then the namespace will be dropped from
	// the GV registry (attended feature to avoid empty namespaces in UE)
	if (!JsonNamespace->TryGetArrayField(TEXT("Variables"), varsJson))
		return;

	JsonNamespace->TryGetStringField(TEXT("Namespace"), GVNamespace.Namespace);
	JsonNamespace->TryGetStringField(TEXT("Description"), GVNamespace.Description);

	for (const auto varJson : *varsJson)
	{
		const auto obj = varJson->AsObject();
		if (!obj.IsValid())
			continue;

		FArticyGVar var;
		GetGVVariable(var, obj);
		GVNamespace.Variables.Add(var);
	}
}

void ADJsonFileBuilder::GetGVVariable(FArticyGVar& var, const TSharedPtr<FJsonObject> JsonVar)
{
	if (!JsonVar.IsValid())
		return;

	JsonVar->TryGetStringField(TEXT("Variable"), var.Variable);
	JsonVar->TryGetStringField(TEXT("Description"), var.Description);

	FString typeString;
	if (JsonVar->TryGetStringField(TEXT("Type"), typeString))
	{
		if (typeString == TEXT("Boolean"))
			var.Type = EArticyType::ADT_Boolean;
		else if (typeString == TEXT("Integer"))
			var.Type = EArticyType::ADT_Integer;
		else
		{
			if (typeString != TEXT("String"))
				UE_LOG(LogArticyEditor, Error, TEXT("Unknown GlobalVariable type '%s', falling back to String."),
			       *typeString);

			var.Type = EArticyType::ADT_String;
		}
	}

	switch (var.Type)
	{
	case EArticyType::ADT_Boolean: JsonVar->TryGetBoolField(TEXT("Value"), var.BoolValue);
		break;
	case EArticyType::ADT_Integer: JsonVar->TryGetNumberField(TEXT("Value"), var.IntValue);
		break;
	case EArticyType::ADT_String: JsonVar->TryGetStringField(TEXT("Value"), var.StringValue);
		break;
	default: break;
	}
}

/**************************************************************
		To remove OR implement (not sure it's useful)
**************************************************************/
void ADJsonFileBuilder::GetHierarchy()
{
	UE_LOG(LogArticyEditor, Warning, TEXT("@TodoOrRemove ?.? => Hierarchy import"));
}


