//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithTransform.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithTransform : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Transform' implement this interface.
 */
class IArticyObjectWithTransform : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithTransform")
	virtual UArticyTransformation*& GetTransform()
	{
		static const auto PropName = FName("Transform");
		return GetProperty<UArticyTransformation*>(PropName);
	}

	virtual const UArticyTransformation* GetTransform() const
	{
		return const_cast<IArticyObjectWithTransform*>(this)->GetTransform();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithTransform")
	virtual UArticyTransformation*& SetTransform(UArticyTransformation* Transform)
	{
		return GetTransform() = Transform;
	}
};
