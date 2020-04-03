//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithDisplayName.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithDisplayName : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'DisplayName' implement this interface.
 */
class IArticyObjectWithDisplayName : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithDisplayName")
	virtual FText& GetDisplayName()
	{
		static const auto PropName = FName("DisplayName");
		return GetProperty<FText>(PropName);
	}

	virtual const FText& GetDisplayName() const
	{
		return const_cast<IArticyObjectWithDisplayName*>(this)->GetDisplayName();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithDisplayName")
	virtual FText& SetDisplayName(UPARAM(ref) const FText& DisplayName)
	{
		return GetDisplayName() = DisplayName;
	}
};
