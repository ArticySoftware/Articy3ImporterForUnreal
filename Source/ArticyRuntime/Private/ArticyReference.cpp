//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyReference.h"


UArticyReference::UArticyReference()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UArticyReference::IsValid()
{
	return Reference.GetId().Get() != 0;
}

UArticyPrimitive* UArticyReference::GetObject(const UObject* WorldContext)
{
	return Reference.GetObject<UArticyPrimitive>(WorldContext);
}

void UArticyReference::SetReference(UArticyObject* Object)
{
	Reference.SetReference(Object);
}
