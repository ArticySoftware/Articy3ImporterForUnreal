#pragma once
#include "IFileDispatcher.h"

class UArticyImportData;

/**
 * @brief Used to dispatch data from one Articy Json formatted database
 *			to Unreal serializable assets.
 */
class FJsonFileDispatcher:public IFileDispatcher
{
protected: 
	UArticyImportData* _asset;
	bool bNeedCodeGeneration = false;
	
	virtual void PostImport();
	virtual bool DidObjectDefsOrGVsChange();
	virtual bool DidScriptFragmentsChange();
	
public:
	explicit FJsonFileDispatcher(UArticyImportData* Asset)
	{
		_asset = Asset;
	}
	
	virtual bool HandleFile(FString JsonString) override;
	bool isCodeGenerationNeeded() const {return bNeedCodeGeneration;}
};
