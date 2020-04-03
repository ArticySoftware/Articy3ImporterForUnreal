//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyFlowPlayer.h"
#include "ArticyReflectable.h"
#include "ArticyFlowObject.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UArticyFlowObject : public UArticyReflectable { GENERATED_BODY() };

/**
 * All objects that are part of a flow (i.e. can be traversed by the Flow Player) need
 * to implement this interface.
 */
class IArticyFlowObject
{
	GENERATED_BODY()

public:
	virtual EArticyPausableType GetType() = 0;

	/** Gather all branches that start at this node. */
	virtual void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) = 0;

	/** Executes any script fragments found on this node. */
	virtual void Execute(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) { }
};
