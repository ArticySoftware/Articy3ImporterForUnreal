#include "BinaryFileDispatcher.h"
#include "ArticyEditorModule.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool FBinaryFileDispatcher::HandleFile(FString JsonManifest, JsonFileType fileType)
{
	bool triggerReimport = false;
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonManifest);
	
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		if (fileType == manifest)
		{
			_asset->_jsonManifest = JsonParsed; //->GetObjectField();

			// => todo :
			//			sort each hashs from sections of the manifest
			//			to determine which file have to be reimported.
			triggerReimport = true;
		}

		// ??...
		// _asset->_jsonPackageDefs;
		// _asset->_jsonHierarchy;
		// _asset->_jsonUserMethods;

		if (fileType == globalVars)
		{
			_asset->_jsonGlobalVars;
		}

		if (fileType == ObjectDefinitions)
		{
			_asset->_jsonObjectDefinitions;
		}

		if (triggerReimport)
		{
			_asset->MainImportFromDispatcher();
			return true;
		}
	}
	
	return false;
}
