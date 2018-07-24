//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithExternalId.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithExternalId : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'ExternalId' implement this interface.
 */
class IArticyObjectWithExternalId : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithExternalId")
	virtual FString& GetExternalId()
	{
		static const auto PropName = FName("ExternalId");
		return GetProperty<FString>(PropName);
	}

	virtual const FString& GetExternalId() const
	{
		return const_cast<IArticyObjectWithExternalId*>(this)->GetExternalId();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithExternalId")
	virtual FString& SetExternalId(UPARAM(ref) const FString& ExternalId)
	{
		return GetExternalId() = ExternalId;
	}
};
