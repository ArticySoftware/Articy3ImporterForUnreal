//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyFlowPlayer.h"
#include "ArticyConditionProvider.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UArticyConditionProvider : public UInterface { GENERATED_BODY() };

/**
 * Objects that can contain a condition implement this interface.
 */
class IArticyConditionProvider
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Condition", meta=(AdvancedDisplay="GV, MethodProvider"))
	virtual bool Evaluate(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) { return true; }

	/*virtual bool Execute(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr)
	{
		//evaluate condition and return result
		return Evaluate(GV, MethodProvider);
	}*/
};
