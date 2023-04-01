#include "JsonFileDispatcher.h"

#include "ArticyEditorModule.h"
#include "ImportFactories/Data/ArticyImportData.h"
#include "ImportFactories/Data/Hierarchy/ADIHierarchyProxy.h"
#include "ImportFactories/Data/PackageDefs/ArticyPackageDefsProxy.h"
#include "ImportFactories/Data/Settings/ADISettingsProxy.h"
#include "ImportFactories/Data/Project/ArticyProjectDefProxy.h"
#include "ImportFactories/Data/UserMethods/AIDUserMethodsProxy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool FJsonFileDispatcher::HandleFile(FString JsonString)
{
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
#pragma	NOTE( TODO - FJsonFileDispatcher::HandleFile : mod ADISettingsProxy to handle partial regen)		
		_asset->Settings = MakeShared<ADISettingsProxy>()
							->fromJson(JsonParsed->GetObjectField(JSON_SECTION_SETTINGS));

		_asset->Project = MakeShared<FArticyProjectDefProxy>()
							->fromJson(JsonParsed->GetObjectField(JSON_SECTION_PROJECT));

		_asset->PackageDefs = MakeShared<ArticyPackageDefsProxy>()
							->fromJson(&JsonParsed->GetArrayField(JSON_SECTION_PACKAGES));

		_asset->Hierarchy = MakeShared<ADIHierarchyProxy>()
							->fromJson(_asset, JsonParsed->GetObjectField(JSON_SECTION_HIERARCHY));

		_asset->UserMethods = MakeShared<AIDUserMethodsProxy>()
							->fromJson(&JsonParsed->GetArrayField(JSON_SECTION_SCRIPTMEETHODS));
		
		_asset->ParentChildrenCache.Empty();
		
		if(DidObjectDefsOrGVsChange())
		{
			// todo : GV && ObjectDef
			// GlobalVariables.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_GLOBALVARS), this);
			// ObjectDefinitions.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_OBJECTDEFS), this);
			bNeedCodeGeneration = true;
		}
		
		if(DidScriptFragmentsChange() && _asset->Settings.set_UseScriptSupport)
		{
			// todo : scripts generation (create special proxy for that / externalyze
			// this->GatherScripts();
			bNeedCodeGeneration = true;
		}
	}
	else return false;

	return true;
}

bool FJsonFileDispatcher::DidObjectDefsOrGVsChange()
{
	return _asset->Settings.ObjectDefinitionsHash != _asset->ObjectDefinitionsHash;
}

bool FJsonFileDispatcher::DidScriptFragmentsChange()
{
	return _asset->Settings.ScriptFragmentsHash != _asset->ScriptFragmentsHash;
}

void FJsonFileDispatcher::PostImport()
{
	FArticyEditorModule& ArticyEditorModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>(
	"ArticyEditor");

#pragma	NOTE( TODO - FJsonFileDispatcher::PostImport : broadcast import finished)
	// ArticyEditorModule.OnImportFinished.Broadcast();
}
