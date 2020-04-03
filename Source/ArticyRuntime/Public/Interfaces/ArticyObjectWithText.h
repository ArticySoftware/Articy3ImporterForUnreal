//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithText.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithText : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Text' implement this interface.
 */
class IArticyObjectWithText : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithText")
	virtual FText& GetText()
	{
		static const auto PropName = FName("Text");
		return GetProperty<FText>(PropName);
	}

	virtual const FText& GetText() const
	{
		return const_cast<IArticyObjectWithText*>(this)->GetText();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithText")
	virtual FText& SetText(UPARAM(ref) const FText& Text)
	{
		return GetText() = Text;
	}
};
