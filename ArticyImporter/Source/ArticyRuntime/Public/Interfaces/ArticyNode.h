//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
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

	EArticyPausableType GetType() override { ensure(false); return static_cast<EArticyPausableType>(0); }

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;
};
