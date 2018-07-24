//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithSize.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithSize : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Size' implement this interface.
 */
class IArticyObjectWithSize : public IArticyObjectWith_Base 
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithSize")
	virtual FArticySize& GetSize()
	{
		static const auto PropName = FName("Size");
		return GetProperty<FArticySize>(PropName);
	}

	virtual const FArticySize& GetSize() const
	{
		return const_cast<IArticyObjectWithSize*>(this)->GetSize();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithSize")
	virtual FArticySize& SetSize(UPARAM(ref) const FArticySize& Size)
	{
		return GetSize() = Size;
	}
};
