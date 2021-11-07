//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//
#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "ArticyAlternativeGVFactory.generated.h"


/**
 * Implements a factory for UArticyAlternativeGlobalVariable assets.
 * Made heavy use of this tutorial: https://gmpreussner.com/reference/adding-new-asset-types-to-ue4
 */
UCLASS(hidecategories=Object)
class UArticyAlternativeGVFactory
	: public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
};