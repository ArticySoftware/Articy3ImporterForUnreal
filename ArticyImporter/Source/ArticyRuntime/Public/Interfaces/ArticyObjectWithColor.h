//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithColor.generated.h"

UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithColor : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Color' implement this interface.
 */
class IArticyObjectWithColor : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithColor")
	virtual FLinearColor& GetColor()
	{
		static const auto PropName = FName("Color");
		return GetProperty<FLinearColor>(PropName);
	}

	virtual FLinearColor& GetColor() const
	{
		return const_cast<IArticyObjectWithColor*>(this)->GetColor();
	}

	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithColor")
	virtual FLinearColor& SetColor(UPARAM(ref) const FLinearColor& Color)
	{
		return GetColor() = Color;
	}
};
