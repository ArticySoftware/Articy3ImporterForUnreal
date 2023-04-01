#pragma once
#include "ArticyPackageDefs.h"

class ArticyPackageDefsProxy
{
	FArticyPackageDefs _packageDefs;
public:
	FArticyPackageDefs fromJson(const TArray<TSharedPtr<FJsonValue>>* Json);
	
	// Scripts and Assets generation relative...
	//		=> Todo simplify/refacto...
	// void GatherScripts(UArticyImportData* Data) const;
	// void GenerateAssets(UArticyImportData* Data) const;//MM_CHANGE
	// TSet<FString> GetPackageNames() const;
	
private: 
	void GetPackageDef(FArticyPackageDef &pDef, const TSharedPtr<FJsonObject> Json);
	void GetModelDef(FArticyModelDef &modelDef, const TSharedPtr<FJsonObject> Json);
	EArticyAssetCategory GetAssetCategoryFromString(const FString Category);
};
