//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
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
