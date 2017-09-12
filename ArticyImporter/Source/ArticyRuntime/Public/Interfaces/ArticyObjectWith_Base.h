//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyReflectable.h"
#include "ArticyObjectWith_Base.generated.h"

UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWith_Base : public UArticyReflectable { GENERATED_BODY() };

/**
 * All objects that have a property called 'Color' implement this interface.
 * The interfaces deriving from this one usually have three methods:
 * 
 * - UFUNCTION getter returning a non-const reference
 * - const getter returning a const reference
 * - UFUNCTION setter returning a non-const reference (as returning a const-reference
 *														does not work for blueprints!)
 *													
 * If IDs are involved, there are getters/setters working on the pointed-to
 * objects for convenience.
 */
class IArticyObjectWith_Base : public IArticyReflectable
{
	GENERATED_BODY()

protected:

	template<typename PropType>
	PropType& GetProperty(const FName& PropName)
	{
		auto prop = GetPropPtr<PropType>(PropName);

		if(ensure(prop))
			return *prop;
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot get property %s from object %s!"),
				   *PropName.ToString(), _getUObject() ? *_getUObject()->GetName() : TEXT("(nullptr)"));
		}

		static PropType Empty;
		return Empty;
	}
};
