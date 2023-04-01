#include "ADIHierarchyProxy.h"

#include "Dom/JsonObject.h"
#include "ImportFactories/Data/ArticyImportData.h"

FADIHierarchy ADIHierarchyProxy::fromJson(UArticyImportData* ImportData, const TSharedPtr<FJsonObject> Json)
{
	_hierarchy.RootObject = nullptr;

	if (!Json.IsValid()) return {};

	_hierarchy.RootObject = recursiveGetHierarchyFromJson(ImportData, Json);

	return _hierarchy;
}

UADIHierarchyObject* ADIHierarchyProxy::recursiveGetHierarchyFromJson(UObject* Outer, const TSharedPtr<FJsonObject> JsonObject)
{
	if (!JsonObject.IsValid())
		return {};

	// Alewinn : object MUST have been created before being serialized by Unreal, their life cycle
	//			 is then handled by Unreal's garbage collector system (use of "NewObject").
	UADIHierarchyObject* obj = NewObject<UADIHierarchyObject>(Outer, UADIHierarchyObject::StaticClass());
	JsonObject->TryGetStringField(TEXT("Type"), obj->Id);
	JsonObject->TryGetStringField(TEXT("TechnicalName"), obj->TechnicalName);
	JsonObject->TryGetStringField(TEXT("Type"), obj->Type);

	// Recurse to childrens => Stack can grow large in memory due to stacking/unstacking op.
	const TArray<TSharedPtr<FJsonValue>> *jsonChildren;
	if (JsonObject->TryGetArrayField(TEXT("Children"), jsonChildren) && jsonChildren)
	{
		obj->Children.Reset(jsonChildren->Num());
		for (auto jsonChild : *jsonChildren)
		{
			auto child = recursiveGetHierarchyFromJson(obj, jsonChild->AsObject());
			obj->Children.Add(child);
		}
	}

	return obj;
}
