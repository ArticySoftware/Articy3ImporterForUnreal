#pragma once
#include "IFileDispatcher.h"
#include "ImportFactories/Data/ArticyImportData.h"

class UArticyImportData;

class FJsonFileDispatcher:public IFileDispatcher
{
	UArticyImportData* _asset;

public:
	explicit FJsonFileDispatcher(UArticyImportData* Asset)
	{
		_asset = Asset;
	}

	virtual void HandleManifest(FString JsonManifest) override;
};
