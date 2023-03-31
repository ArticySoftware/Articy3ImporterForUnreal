#include "JsonFileDispatcher.h"

#include "ImportFactories/Data/ArticyImportData.h"
#include "ImportFactories/Data/Proxies/ADISettingsProxy.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool FJsonFileDispatcher::HandleFile(FString JsonString)
{
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		_asset->Settings = MakeShared<ADISettingsProxy>()
							->fromJson(JsonParsed->GetObjectField(JSON_SECTION_SETTINGS));
		
		// ADJsonFileBuilder::BuildAsset(*_asset,JsonParsed);
	}
	else return false;

	return true;
}
