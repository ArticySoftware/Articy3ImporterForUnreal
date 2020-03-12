//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyPins.h"
#include "ArticyFlowObject.h"
#include "ArticyInputPinsProvider.generated.h"

UINTERFACE()
class UArticyInputPinsProvider : public UArticyFlowObject { GENERATED_BODY() };

/**
 * All objects that have a property called 'InputPins' (which is an array of UArticyInputPin*)
 * implement this interface.
 */
class IArticyInputPinsProvider : public IArticyFlowObject
{
	GENERATED_BODY()

public:

	/**
	 * Tries to submerge into InputPins and explore there.
	 * Returns false if no InputPins are found, or if none of them has connections.
	 * NOTE: This method is only needed if an exploration starts at a flow node. Later (or if
	 * it starts on a pin), exploration of pins will happen automatically as they are reached.
	 */
	bool TrySubmerge(class UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth, const bool bForceShadowed)
	{
		bool bSubmerged = false;

		auto inPins = GetInputPins();
		if(ensure(inPins) && inPins->Num() > 0)
		{
			//if there is more than one pin or the single pin has more connections,
			//it must be a shadowed explore
			const auto bShadowed = bForceShadowed
									|| inPins->Num() > 1
									|| ((*inPins)[0] && (*inPins)[0]->Connections.Num() > 1);

			//submerge!
			for(auto pin : *inPins)
			{
				//skip pins with no connections, since non-submergeable pins should not exist if
				//at least one of the other pins can be submerged
				//if none of the pins has connections, TrySubmerge will fail anyways, and the owner will be explored instead
				if(ensure(pin) && pin->Connections.Num() > 0)
				{
					bSubmerged = true;
					OutBranches.Append( Player->Explore(pin, bShadowed, Depth+1) );
				}
			}
		}

		return bSubmerged;
	}
	
	const TArray<UArticyInputPin*>* GetInputPins() const
	{
		const static auto name = FName("InputPins");
		return Cast<IArticyReflectable>(this)->GetPropPtr<TArray<UArticyInputPin*>>(name);
	}
};
