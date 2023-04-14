#include "HierarchyObject.h"

#include "ArticyHelpers.h"

UADIHierarchyObject* UADIHierarchyObject::CreateFromJson(UObject* Outer, const TSharedPtr<FJsonObject> JsonObject)
{
	if (!JsonObject.IsValid())
		return nullptr;

	//extract properties
	FString Id;
	JSON_TRY_STRING(JsonObject, Id);
	FString TechnicalName;
	JSON_TRY_STRING(JsonObject, TechnicalName);
	FString Type;
	JSON_TRY_STRING(JsonObject, Type);

	//create new object, referenced by Outer (otherwise it is transient!), with name TechnicalName
	auto obj = NewObject<UADIHierarchyObject>(Outer, *TechnicalName);
	obj->Id = Id;
	obj->TechnicalName = TechnicalName;
	obj->Type = Type;

	//fill in children
	const TArray<TSharedPtr<FJsonValue>>* jsonChildren;
	if (JsonObject->TryGetArrayField(TEXT("Children"), jsonChildren) && jsonChildren)
	{
		obj->Children.Reset(jsonChildren->Num());
		for (auto jsonChild : *jsonChildren)
		{
			auto child = CreateFromJson(obj, jsonChild->AsObject());
			obj->Children.Add(child);
		}
	}

	return obj;
}
