#pragma once
#include "IFileDispatcher.h"

class UArticyImportData;

class FJsonFileDispatcher:public IFileDispatcher
{
protected: 
	UArticyImportData* _asset;
	
public:
	explicit FJsonFileDispatcher(UArticyImportData* Asset)
	{
		_asset = Asset;
	}

	virtual bool HandleFile(FString JsonString) override;
};
