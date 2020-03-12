//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithShortId.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithShortId : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'ShortId' implement this interface.
 */
class IArticyObjectWithShortId : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithShortId")
	virtual int32& GetShortId()
	{
		static const auto PropName = FName("ShortId");
		return GetProperty<int32>(PropName);
	}

	virtual const int32& GetShortId() const
	{
		return const_cast<IArticyObjectWithShortId*>(this)->GetShortId();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithShortId")
	virtual int32& SetShortId(UPARAM(ref) const int32& ShortId)
	{
		return GetShortId() = ShortId;
	}
};
