#pragma once
#include "Hierarchy.generated.h"

class UArticyImportData;
class UADIHierarchyObject;

USTRUCT()
struct FADIHierarchy
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="Hierarchy")
	UADIHierarchyObject* RootObject = nullptr;

	void ImportFromJson(UArticyImportData* ImportData, const TSharedPtr<FJsonObject> JsonRoot);
};