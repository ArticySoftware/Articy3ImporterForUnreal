//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once



#include "Interfaces/ArticyNode.h"

void UArticyNode::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	//default implementation: continue on output pins
	IArticyOutputPinsProvider::Explore(Player, OutBranches, Depth + 1);
}
