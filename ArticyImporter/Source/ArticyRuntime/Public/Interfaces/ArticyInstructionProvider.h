//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyFlowPlayer.h"
#include "ArticyInstructionProvider.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UArticyInstructionProvider : public UInterface { GENERATED_BODY() };

/**
 * Objects that can contain an instruction implement this interface.
 */
class IArticyInstructionProvider
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Instruction")
	virtual void Execute(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) { }
};
