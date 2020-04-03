//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyPins.h"
#include "ArticyFlowObject.h"
#include "ArticyOutputPinsProvider.generated.h"

UINTERFACE()
class ARTICYRUNTIME_API UArticyOutputPinsProvider : public UArticyFlowObject { GENERATED_BODY() };

/**
 * All objects that have a property called 'OutputPins' (which is an array of UArticyInputPin*)
 * implement this interface.
 */
class ARTICYRUNTIME_API IArticyOutputPinsProvider : public IArticyFlowObject
{
	GENERATED_BODY()

public:

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;

	const TArray<UArticyOutputPin*>* GetOutputPinsPtr() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<UArticyOutputPin*> GetOutputPins() const;

	TArray<UArticyOutputPin*> GetOutputPins_Implementation() const;
};
