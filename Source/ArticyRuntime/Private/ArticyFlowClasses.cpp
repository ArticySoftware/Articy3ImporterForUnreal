//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//


#include "ArticyFlowClasses.h"
//#include "ArticyBaseTypes.h"

UArticyPrimitive* UArticyJump::GetTarget() const
{
	if (!TargetObj)
	{
		auto db = UArticyDatabase::Get(this);
		TargetObj = db ? db->GetObject(Target) : nullptr;
	}

	return TargetObj;
}

UArticyFlowPin* UArticyJump::GetTargetPin() const
{
	if (!TargetPinObj)
	{
		auto target = GetTarget();
		if (target)
			TargetPinObj = Cast<UArticyFlowPin>(target->GetSubobject(TargetPin));
	}

	return TargetPinObj;
}

void UArticyJump::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	//NOTE: Even though a Jump also has an OutputPins member because of inheritance, it never really has output pins!
	//Instead, the jump target is specified by the TargetPin (and Target) member.

	auto pin = GetTargetPin();
	if(pin)
	{
		const auto bShadowed = false;
		OutBranches.Append( Player->Explore(pin, bShadowed, Depth + 1) );
	}
	else
	{
		//DEAD-END!
		OutBranches.Add(FArticyBranch{});
	}
}
