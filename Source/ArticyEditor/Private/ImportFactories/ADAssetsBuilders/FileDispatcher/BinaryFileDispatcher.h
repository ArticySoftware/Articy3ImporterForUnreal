#pragma once
#include "IFileDispatcher.h"
#include "ImportFactories/Data/ArticyImportData.h"

class UArticyImportData;

class FBinaryFileDispatcher:public IFileDispatcher
{
	UArticyImportData* _asset;
	bool triggerReimport;
	
public:

	explicit FBinaryFileDispatcher(UArticyImportData* Asset)
	{
		_asset = Asset;
		triggerReimport = false;
	}
	
	virtual bool HandleFile(FString JsonManifest, JsonFileType fleType) override;
	virtual void HandleReimport() override;
};
