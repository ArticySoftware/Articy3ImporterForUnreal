#pragma once
#include "ADIHierarchy.h"

class UArticyImportData;

class ADIHierarchyProxy
{
	FADIHierarchy _hierarchy;

public:
	FADIHierarchy fromJson(UArticyImportData* ImportData, const TSharedPtr<FJsonObject> Json);

private :
	UADIHierarchyObject* recursiveGetHierarchyFromJson(UObject* Outer,const TSharedPtr<FJsonObject> JsonObject);
};
