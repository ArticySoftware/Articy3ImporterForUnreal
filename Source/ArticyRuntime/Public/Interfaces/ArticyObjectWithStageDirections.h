//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithStageDirections.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithStageDirections : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'StageDirections' implement this interface.
 */
class IArticyObjectWithStageDirections : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithStageDirections")
	virtual FText& GetStageDirections()
	{
		static const auto PropName = FName("StageDirections");
		return GetProperty<FText>(PropName);
	}
	
	virtual FText GetStageDirections() const
	{
		return const_cast<IArticyObjectWithStageDirections*>(this)->GetStageDirections();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithStageDirections")
	virtual FText& SetStageDirections(UPARAM(ref) const FText& StageDirections)
	{
		return GetStageDirections() = StageDirections;
	}
};
