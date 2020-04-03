//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithTarget.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithTarget : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Target' implement this interface.
 */
class IArticyObjectWithTarget : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithTarget")
	virtual UArticyPrimitive* GetTarget() const
	{
		auto id = GetTargetId();
		return id.GetObject(_getUObject());
	}

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithTarget")
	virtual FArticyId& GetTargetId()
	{
		static const auto PropName = FName("Target");
		return GetProperty<FArticyId>(PropName);
	}

	virtual const FArticyId& GetTargetId() const
	{
		return const_cast<IArticyObjectWithTarget*>(this)->GetTargetId();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithTarget")
	virtual UArticyPrimitive* SetTarget(UArticyPrimitive* const Target)
	{
		SetTargetId(Target ? Target->GetId() : (FArticyId) 0);
		return Target;
	}

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithTarget")
	virtual FArticyId& SetTargetId(UPARAM(ref) const FArticyId& Id)
	{
		return GetTargetId() = Id;
	}
};
