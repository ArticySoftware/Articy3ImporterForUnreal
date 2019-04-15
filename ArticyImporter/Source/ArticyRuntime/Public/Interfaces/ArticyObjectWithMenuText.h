//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithMenuText.generated.h"

UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithMenuText : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'MenuText' implement this interface.
 */
class IArticyObjectWithMenuText : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithMenuText")
	virtual FText& GetMenuText()
	{
		static auto MenuText = FName("MenuText");
		return GetProperty<FText>(MenuText);
	}

	virtual const FText& GetMenuText() const
	{
		return const_cast<IArticyObjectWithMenuText*>(this)->GetMenuText();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithMenuText")
	virtual FText& SetMenuText(UPARAM(ref) const FText& MenuText)
	{
		return GetMenuText() = MenuText;
	}
};
