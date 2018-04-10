//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "ArticyObject.h"
#include "ArticyHelpers.h"
#include "ArticyDatabase.h"
#include "ArticyBaseTypes.h"

void UArticyObject::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if(!Json.IsValid() || Json->Type != EJson::Object)
		return;

	auto obj = Json->AsObject();
	if(!ensure(obj.IsValid()))
		return;

	JSON_TRY_HEX_ID(obj, Parent);
	JSON_TRY_STRING(obj, TechnicalName);
}

//---------------------------------------------------------------------------//

FName UArticyObject::GetTechnicalName() const
{
	return *TechnicalName;
}

UArticyObject* UArticyObject::GetParent() const
{
	return UArticyDatabase::Get(this)->GetObject<UArticyObject>(Parent);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
TArray<UArticyObject*> UArticyObject::GetChildren() const
{
	return TArray<UArticyObject*>();
}

UArticyBaseFeature* UArticyObject::GetFeature(FName propertyName)
{
	auto propPtr = GetPropPtr<UArticyBaseFeature *>(propertyName);
	return propPtr ? *propPtr : nullptr;
}