//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObject.h"
#include "ArticyFlowObject.h"
#include "ArticyInputPinsProvider.h"
#include "ArticyOutputPinsProvider.h"

#include "ArticyNode.generated.h"

/**
 * All nodes (fragments) are derived from this base class.
 */
UCLASS(abstract)
class ARTICYRUNTIME_API UArticyNode : public UArticyObject, public IArticyInputPinsProvider, public IArticyOutputPinsProvider
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EArticyPausableType GetType() override { ensure(false); return static_cast<EArticyPausableType>(0); }

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;
};
