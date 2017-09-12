//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "ArticyPrimitive.h"
#include "ArticyBaseObject.h"

UArticyPrimitive* UArticyBaseObject::GetSubobject(FArticyId Id) const
{
	auto obj = Subobjects.Find(Id);
	return obj ? *obj : nullptr;
}

void UArticyBaseObject::AddSubobject(UArticyPrimitive* Obj)
{
	Subobjects.Add(Obj->GetId(), Obj);
}
