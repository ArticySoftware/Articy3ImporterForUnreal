//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "ArticyPins.h"
#include "ArticyFlowObject.h"
#include "ArticyInputPinsProvider.generated.h"

UINTERFACE()
class ARTICYRUNTIME_API UArticyInputPinsProvider : public UArticyFlowObject { GENERATED_BODY() };

/**
 * All objects that have a property called 'InputPins' (which is an array of UArticyInputPin*)
 * implement this interface.
 */
class ARTICYRUNTIME_API IArticyInputPinsProvider : public IArticyFlowObject
{
	GENERATED_BODY()

public:

	/**
	 * Tries to submerge into InputPins and explore there.
	 * Returns false if no InputPins are found, or if none of them has connections.
	 * NOTE: This method is only needed if an exploration starts at a flow node. Later (or if
	 * it starts on a pin), exploration of pins will happen automatically as they are reached.
	 */
	bool TrySubmerge(class UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth, const bool bForceShadowed);

	const TArray<UArticyInputPin*>* GetInputPinsPtr() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Articy")
	TArray<UArticyInputPin*> GetInputPins() const;

	TArray<UArticyInputPin*> GetInputPins_Implementation() const;
};
