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

	enum JsonFileType
	{
		manifest,
		globalVars,
		objectDefinitions,
		objectDefinitionsLocalization,
		hierarchy,
		package,
		scriptMethods
	};
	
	virtual bool HandleFile(FString JsonManifest, JsonFileType fleType) = 0;
	virtual void HandleReimport() = 0;
};
