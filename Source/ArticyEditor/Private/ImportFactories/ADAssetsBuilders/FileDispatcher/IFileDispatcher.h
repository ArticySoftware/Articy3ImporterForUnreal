#pragma once

class UArticyImportData;

/**
 * @brief A FileDispatcher is in charge to get Json data as input and map theses
 *			data inside a given unreal serializable Asset format (most of the
 *			time derivated from UDataAsset) passed to child classes inside
 *			their constructor.
 */
class IFileDispatcher
{
public:
	// To complete when able to use full ArticyX export... 
	enum JsonFileType
	{
		manifest,
		globalVars,
		ObjectDefinitions
	};
	
	virtual bool HandleFile(FString JsonManifest, JsonFileType fleType) = 0;
};
