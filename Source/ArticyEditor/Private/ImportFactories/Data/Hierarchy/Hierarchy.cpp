#include "Hierarchy.h"

#include "HierarchyObject.h"
#include "ImportFactories/Data/ArticyImportData.h"

void FADIHierarchy::ImportFromJson(UArticyImportData* ImportData, const TSharedPtr<FJsonObject> Json)
{
	RootObject = nullptr;

	//find the "Hierarchy" section
	if (!Json.IsValid())
		return;

	RootObject = UADIHierarchyObject::CreateFromJson(ImportData, Json);
}
