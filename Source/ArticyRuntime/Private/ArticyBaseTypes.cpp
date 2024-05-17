//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyBaseTypes.h"
#include "ArticyHelpers.h"
#include "ArticyDatabase.h"

bool FArticyId::InitFromString(const FString InSourceString)
{
	Low = High = 0;

	const bool bSuccessful = FParse::Value(*InSourceString, TEXT("Low="), Low) && FParse::Value(*InSourceString, TEXT("High="), High);

	return bSuccessful;
}

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
	w = obj->GetNumberField(TEXT(STRINGIFY(w)));
	h = obj->GetNumberField(TEXT(STRINGIFY(h)));
}

FArticyRect::FArticyRect(TSharedPtr<FJsonValue> Json)
{
	if(!ensure(Json.IsValid() && Json->Type == EJson::Object))
		return;

	auto obj = Json->AsObject();

	x = obj->GetNumberField(TEXT(STRINGIFY(x)));
	y = obj->GetNumberField(TEXT(STRINGIFY(y)));
	w = obj->GetNumberField(TEXT(STRINGIFY(w)));
	h = obj->GetNumberField(TEXT(STRINGIFY(h)));
}
