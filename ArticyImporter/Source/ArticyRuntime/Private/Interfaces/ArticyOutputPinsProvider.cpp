//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyRuntimePrivatePCH.h"

#include "ArticyOutputPinsProvider.h"

void IArticyOutputPinsProvider::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	auto pins = GetOutputPins();
	if(ensure(pins) && pins->Num() > 0)
	{
		//shadow needed?
		const auto bShadowed = pins->Num() > 1;

		for(auto pin : *pins)
			OutBranches.Append( Player->Explore(pin, bShadowed, Depth + 1) );
	}
	else
	{
		//DEAD-END!
		OutBranches.Add(FArticyBranch{});
	}
}

const TArray<UArticyOutputPin*>* IArticyOutputPinsProvider::GetOutputPins() const
{
	const static auto name = FName("OutputPins");
	return GetPropPtr<TArray<UArticyOutputPin*>>(name);
}
