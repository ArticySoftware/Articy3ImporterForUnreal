//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ShadowStateManager.h"

void IShadowStateManager::UnregisterOnPopState(FDelegateHandle Delegate)
{
	OnPopStateDelegates.Last().Remove(Delegate);
}

void IShadowStateManager::PushState(uint32 NewShadowLevel)
{
	//create a new delegate just for this new shadow state
	OnPopStateDelegates.Emplace();
	++ShadowLevel;

	ensureMsgf(ShadowLevel == NewShadowLevel, TEXT("ShadowLevels do not match in PushState!"));
}

void IShadowStateManager::PopState(uint32 CurrShadowLevel)
{
	ensureMsgf(ShadowLevel == CurrShadowLevel, TEXT("ShadowLevels do not match in PopState!"));

	if(ensureMsgf(OnPopStateDelegates.Num() > 0, TEXT("InPopStateDelegates empty while popping a state!")))
	{
		//notify only the variables that registered during THIS operation
		auto onPop = OnPopStateDelegates.Pop();
		if(onPop.IsBound())
			onPop.Broadcast();
	}

	if(ensureMsgf(ShadowLevel > 0, TEXT("Trying to pop state when ShadowLevel is 0!")))
		--ShadowLevel;
}
