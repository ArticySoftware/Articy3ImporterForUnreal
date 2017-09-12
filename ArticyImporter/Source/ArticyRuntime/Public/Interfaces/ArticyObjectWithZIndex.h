//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithZIndex.generated.h"

UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithZIndex : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'ZIndex' implement this interface.
 */
class IArticyObjectWithZIndex : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithZIndex")
	virtual float& GetZIndex()
	{
		static const auto PropName = FName("ZIndex");
		return GetProperty<float>(PropName);
	}

	virtual const float& GetZIndex() const
	{
		return const_cast<IArticyObjectWithZIndex*>(this)->GetZIndex();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithZIndex")
	virtual float& SetZIndex(UPARAM(ref) const float& ZIndex)
	{
		return GetZIndex() = ZIndex;
	}
};
