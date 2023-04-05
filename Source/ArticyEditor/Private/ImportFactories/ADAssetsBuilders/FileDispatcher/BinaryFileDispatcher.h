#pragma once
#include "IFileDispatcher.h"
#include "ImportFactories/ArticyImportData.h"
// #include "ImportFactories/Data/ArticyImportData.h"

class UArticyImportData;

class FBinaryFileDispatcher:public IFileDispatcher
{
	UArticyImportData* _asset;

public:

	explicit FBinaryFileDispatcher(UArticyImportData* Asset)
	{
		_asset = Asset;
	}
	
	virtual bool HandleFile(FString JsonManifest, JsonFileType fleType) override;
};
