//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//


#include "ArticyBaseTypes.h"
#include "ArticyHelpers.h"
#include "ArticyDatabase.h"

UArticyPrimitive* FArticyId::GetObject(const UObject* WorldContext) const
{
	auto db = UArticyDatabase::Get(WorldContext);
	return db ? db->GetObject(*this) : nullptr;
}

FArticySize::FArticySize(TSharedPtr<FJsonValue> Json)
{
	if(!ensure(Json.IsValid() && Json->Type == EJson::Object))
		return;

	auto obj = Json->AsObject();
	w = obj->GetNumberField(STRINGIFY(w));
	h = obj->GetNumberField(STRINGIFY(h));
}

FArticyRect::FArticyRect(TSharedPtr<FJsonValue> Json)
{
	if(!ensure(Json.IsValid() && Json->Type == EJson::Object))
		return;

	auto obj = Json->AsObject();

	x = obj->GetNumberField(STRINGIFY(x));
	y = obj->GetNumberField(STRINGIFY(y));
	w = obj->GetNumberField(STRINGIFY(w));
	h = obj->GetNumberField(STRINGIFY(h));
}
