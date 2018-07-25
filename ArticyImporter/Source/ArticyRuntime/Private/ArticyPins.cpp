//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"
#include "ArticyPins.h"
#include "ArticyBaseTypes.h"
#include "ArticyBuiltinTypes.h"
#include "ArticyExpressoScripts.h"

void UArticyFlowPin::InitFromJson(TSharedPtr<FJsonValue> Json) 
{
	Super::InitFromJson(Json);

	if (!Json.IsValid() || !ensure(Json->Type == EJson::Object))
		return;

	auto obj = Json->AsObject();
	JSON_TRY_FNAME(obj, Text);

	auto id = obj->TryGetField("Owner");
	Owner = FArticyId{id};

	const TArray<TSharedPtr<FJsonValue>>* items;
	if (obj->TryGetArrayField(TEXT("Connections"), items))
	{
		for (int i = 0; i < items->Num(); ++i)
		{
			auto item = (*items)[i];
			auto conn = NewObject<UArticyOutgoingConnection>(this, *FString::Printf(TEXT("Connection_%d'"), i));
			conn->InitFromJson(item);
			Connections.Add(conn);
		}
	}
}

UArticyObject* UArticyFlowPin::GetOwner()
{
	auto db = UArticyDatabase::Get(this);
	return ensure(db) ? db->GetObject<UArticyObject>(Owner) : nullptr;
}

//---------------------------------------------------------------------------//

bool UArticyInputPin::Evaluate(class UArticyGlobalVariables* GV, class UObject* MethodProvider)
{
	auto db = UArticyDatabase::Get(this);
	return db->GetExpressoInstance()->Evaluate(GetTypeHash(Text), GV ? GV : db->GetGVs(), MethodProvider);
}

void UArticyInputPin::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	//evaluate first, as the evaluate method could have side-effects
	bool bIsValid = Evaluate(Player->GetGVs(), Player->GetMethodsProvider());

	//we can stop here if the branch is invalid and should be ignored
	if(!bIsValid && Player->IgnoresInvalidBranches())
		return;

	IArticyFlowObject* owner = Cast<IArticyFlowObject>(GetOwner());

	if(Depth > 3 && Player->ShouldPauseOn(owner))
	{
		// if the owner of this input pin is a stop node, we directly continue with it instead of submerging
		OutBranches.Append(Player->Explore(owner, false, Depth + 1));
	}
	else if(Connections.Num() > 0)
	{
		//shadow needed?
		const auto bShadowed = Connections.Num() > 1;

		//submerge
		for(auto conn : Connections)
		{
			auto target = conn->GetTargetPin();
			OutBranches.Append( Player->Explore(target, bShadowed, Depth + 1) );
		}
	}
	else
	{
		//no connections, so continue with the owner itself
		OutBranches.Append( Player->Explore(owner, false, Depth+1) );
	}

	/**
	 * All branches that lead THROUGH this pin are invalid if the pin's condition is not valid.
	 * This does not affect a branch that stops AT this pin (as it is only created after this
	 * method exits).
	 */
	if(!bIsValid)
	{
		for(auto& branch : OutBranches)
			branch.bIsValid = false;
	}
}

//---------------------------------------------------------------------------//

void UArticyOutputPin::Execute(class UArticyGlobalVariables* GV, class UObject* MethodProvider)
{
	auto db = UArticyDatabase::Get(this);
	db->GetExpressoInstance()->Execute(GetTypeHash(Text), GV ? GV : db->GetGVs(), MethodProvider);
}

void UArticyOutputPin::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	Execute(Player->GetGVs(), Player->GetMethodsProvider());

	if(Connections.Num() > 0)
	{
		//shadow needed?
		const auto bShadowed = Connections.Num() > 1;

		//branch out
		for(auto conn : Connections)
		{
			auto target = conn->GetTargetPin();
			OutBranches.Append( Player->Explore(target, bShadowed, Depth+1) );
		}
	}
	else
	{
		//DEAD-END!
		OutBranches.Add(FArticyBranch{});
	}
}
