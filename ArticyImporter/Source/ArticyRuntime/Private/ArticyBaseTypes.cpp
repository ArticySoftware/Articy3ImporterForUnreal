//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "ArticyHelpers.h"
#include "ArticyBaseTypes.h"
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
