//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//
#include "ArticyAlternativeGVFactory.h"
#include "ArticyAlternativeGlobalVariables.h"


UArticyAlternativeGVFactory::UArticyAlternativeGVFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UArticyAlternativeGlobalVariables::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UArticyAlternativeGVFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UArticyAlternativeGlobalVariables>(InParent, InClass, InName, Flags);
}


bool UArticyAlternativeGVFactory::ShouldShowInNewMenu() const
{
	return true;
}