//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyBaseObject.h"
#include "ArticyPrimitive.h"

UArticyPrimitive* UArticyBaseObject::GetSubobject(FArticyId Id) const
{
	auto obj = Subobjects.Find(Id);
	return obj ? *obj : nullptr;
}

void UArticyBaseObject::AddSubobject(UArticyPrimitive* Obj)
{
	Subobjects.Add(Obj->GetId(), Obj);
}
