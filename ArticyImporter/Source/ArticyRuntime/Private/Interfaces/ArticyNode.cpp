//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once



#include "ArticyNode.h"

void UArticyNode::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	//default implementation: continue on output pins
	IArticyOutputPinsProvider::Explore(Player, OutBranches, Depth + 1);
}
