#pragma once
#include "JsonFileDispatcher.h"
#include "ImportFactories/Data/ArticyImportData.h"

class UArticyImportData;

class FBinaryFileDispatcher:public FJsonFileDispatcher
{
public:
	explicit FBinaryFileDispatcher(UArticyImportData* Asset)
	:FJsonFileDispatcher(Asset)
	{}
	
	virtual bool HandleFile(FString JsonManifest) override;
};
