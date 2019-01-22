//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
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

void UArticyReference::SetReference(UArticyPrimitive* Object)
{
	Reference.SetReference(Object);
}
