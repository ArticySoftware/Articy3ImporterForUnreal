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
	virtual void HandleManifest(FString JsonManifest) = 0;
};
