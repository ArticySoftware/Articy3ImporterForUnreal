//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyBaseTypes.h"
#include "ArticyObject.h"
#include "ArticyObjectWithSpeaker.generated.h"

struct FArticyId;

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithSpeaker : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Speaker' implement this interface.
 */
class IArticyObjectWithSpeaker : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="ObjectWithSpeaker")
	virtual UArticyObject* GetSpeaker() const
	{
		return GetSpeakerId().GetObject<UArticyObject>(_getUObject());
	}

	UFUNCTION(BlueprintCallable, Category="ObjectWithSpeaker")
	virtual FArticyId& GetSpeakerId()
	{
		static const auto PropName = FName("Speaker");
		return GetProperty<FArticyId>(PropName);
	}

	virtual const FArticyId& GetSpeakerId() const
	{
		return const_cast<IArticyObjectWithSpeaker*>(this)->GetSpeakerId();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ObjectWithSpeaker")
	virtual UArticyObject* SetSpeaker(UArticyObject* const Speaker)
	{
		SetSpeakerId(Speaker ? Speaker->GetId() : (FArticyId) 0);
		return Speaker;
	}

	UFUNCTION(BlueprintCallable, Category="ObjectWithSpeaker")
	virtual FArticyId& SetSpeakerId(UPARAM(ref) const FArticyId& Id)
	{
		return GetSpeakerId() = Id;
	}
};
