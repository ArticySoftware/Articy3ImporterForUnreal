#include "BinaryFileDispatcher.h"
#include "ArticyEditorModule.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool FBinaryFileDispatcher::HandleFile(FString JsonManifest, JsonFileType fileType)
{
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonManifest);

	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		// Command pattern would have been far better...
		// to refacto (later...).
		switch (fileType)
		{
		// => todo :
		//			sort each hashs from sections of the manifest
		//			to determine which file have to be reimported.

		case manifest:

			UE_LOG(LogArticyEditor,Warning,TEXT("=> Manifest found"));
			
			_asset->_jsonManifest = JsonParsed; //->GetObjectField();
			triggerReimport = true;
			break;

		case globalVars:
			UE_LOG(LogArticyEditor,Warning,TEXT("=> Global vars found"));
			
			_asset->_jsonGlobalVars = JsonParsed;
			triggerReimport = true;
			break;

		case objectDefinitions:
			UE_LOG(LogArticyEditor,Warning,TEXT("=> Onject defs found"));			
			
			_asset->_jsonObjectDefinitions = JsonParsed;
			triggerReimport = true;
			break;

		case objectDefinitionsLocalization:
			UE_LOG(LogArticyEditor,Warning,TEXT("=> Object defs loca found"));
			// TODO
			triggerReimport = true;
			break;

		case hierarchy:
			UE_LOG(LogArticyEditor,Warning,TEXT("=> Hierarchy found"));
			
			_asset->_jsonHierarchy = JsonParsed;
			triggerReimport = true;
			break;

		case package:
			UE_LOG(LogArticyEditor,Warning,TEXT("=> Package found"));

			// ?
			_asset->_jsonPackageDefs = JsonParsed;
			triggerReimport = true;
			break;

		case scriptMethods:
			UE_LOG(LogArticyEditor,Warning,TEXT("=> Script methods found"));

			_asset->_jsonUserMethods = JsonParsed;
			triggerReimport = true;
			break;

		default: ;
		}


		return true;
	}

	return false;
}

void FBinaryFileDispatcher::HandleReimport()
{
	if (triggerReimport)
	{
		_asset->MainImportFromDispatcher();
	}
}
