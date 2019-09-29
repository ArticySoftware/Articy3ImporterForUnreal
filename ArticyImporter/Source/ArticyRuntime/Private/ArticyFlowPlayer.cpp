//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyFlowPlayer.h"
#include "ArticyRuntime.h"
#include "ArticyFlowObject.h"
#include "ArticyObjectWithSpeaker.h"
#include "ArticyExpressoScripts.h"
#include "ArticyInputPinsProvider.h"
#include "ArticyOutputPinsProvider.h"

TScriptInterface<IArticyFlowObject> FArticyBranch::GetTarget() const
{
	return Path.Num() > 0 ? Path.Last() : nullptr;
}

void UArticyFlowPlayer::BeginPlay()
{
	Super::BeginPlay();

	//update Cursor to object referenced by StartOn
	SetCursorToStartNode();
}

//---------------------------------------------------------------------------//

void UArticyFlowPlayer::SetStartNode(FArticyRef StartNodeId)
{
	StartOn.SetId(StartNodeId.GetId());
	SetCursorToStartNode();
}

void UArticyFlowPlayer::SetStartNodeWithFlowObject(TScriptInterface<IArticyFlowObject> Node)
{
	FArticyRef ArticyRef;
	ArticyRef.SetReference(Cast<UArticyObject>(Node.GetObject()));
	SetStartNode(ArticyRef);
}

void UArticyFlowPlayer::SetStartNodeById(FArticyId NewId)
{
	StartOn.SetId(NewId);
	SetCursorToStartNode();
}

void UArticyFlowPlayer::SetCursorTo(TScriptInterface<IArticyFlowObject> Node)
{
	if(!Node.GetObject())
	{
		UE_LOG(LogArticyRuntime, Warning, TEXT("Could not set cursor in flow player of actor %s: invalid node"), *this->GetOwner()->GetName());
		return;
	}
	
	Cursor = Node;
	UpdateAvailableBranches();
}

void UArticyFlowPlayer::Play(int BranchIndex)
{
	TArray<FArticyBranch> branches;
	if (IgnoresInvalidBranches())
	{
		for (auto branch : AvailableBranches)
			if (branch.bIsValid)
				branches.Add(branch);
	}
	else
	{
		branches = AvailableBranches;
	}

	//check if the specified branch exists
	if(!branches.IsValidIndex(BranchIndex))
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("Branch with index %d does not exist!"), BranchIndex)
		return;
	}

	PlayBranch(branches[BranchIndex]);
}

void UArticyFlowPlayer::FinishCurrentPausedObject(int PinIndex)
{
	IArticyOutputPinsProvider* outputPinOwner = Cast<IArticyOutputPinsProvider>(Cursor.GetObject());
	if (outputPinOwner)
	{
		auto outputPins = outputPinOwner->GetOutputPinsPtr();

		if (outputPins->Num() > 0)
		{
			if (PinIndex < outputPins->Num())
			{
				(*outputPins)[PinIndex]->Execute(GetGVs(), GetMethodsProvider());
			}
			else
				UE_LOG(LogArticyRuntime, Warning, TEXT("FinishCurrentPausedObject: The index was out of bounds: Index: %d, PinCount: %d"), PinIndex, outputPins->Num());
		}
	}
}

bool UArticyFlowPlayer::ShouldPauseOn(IArticyFlowObject* Node) const
{
	return Node && (1 << static_cast<uint8>(Node->GetType()) & PauseOn);
}

UArticyDatabase* UArticyFlowPlayer::GetDB() const
{
	return UArticyDatabase::Get(this);
}

UArticyGlobalVariables* UArticyFlowPlayer::GetGVs() const
{
	return OverrideGV ? OverrideGV : UArticyGlobalVariables::GetDefault(this);
}

UObject* UArticyFlowPlayer::GetMethodsProvider() const
{
	auto expressoInstance = GetDB()->GetExpressoInstance();
	auto provider = expressoInstance->GetUserMethodsProviderInterface();
	
	if (expressoInstance->UserMethodsProvider != nullptr && UserMethodsProvider == nullptr)//MM_CHANGE
		UserMethodsProvider = expressoInstance->UserMethodsProvider;

	if(ensure(provider))
	{
		//check if the set provider implements the required interface
		if(!UserMethodsProvider || !ensure(UserMethodsProvider->GetClass()->ImplementsInterface(provider)))
		{

			//no valid UserMethodsProvider set, search for it

			//check if the flow player itself implements it
			if(GetClass()->ImplementsInterface(provider))
				UserMethodsProvider = const_cast<UArticyFlowPlayer*>(this);
			else
			{
				auto actor = GetOwner();
				if(ensure(actor))
				{
					//check if the flow player's owning actor implements it
					if(actor->GetClass()->ImplementsInterface(provider))
						UserMethodsProvider = actor;
					else
					{
						//check if any other component implements it
						auto components = actor->GetComponents();
						for(auto comp : components)
						{
							if(comp->GetClass()->ImplementsInterface(provider))
							{
								UserMethodsProvider = comp;
								break;
							}
						}

						//and finally we check for a default methods provider, which we can use as fallback
						auto defaultUserMethodsProvider = expressoInstance->DefaultUserMethodsProvider;
						if(defaultUserMethodsProvider && ensure(defaultUserMethodsProvider->GetClass()->ImplementsInterface(provider)))
							UserMethodsProvider = defaultUserMethodsProvider;
					}
				}
			}
		}
	}

	return UserMethodsProvider;
}

IArticyFlowObject* UArticyFlowPlayer::GetUnshadowedNode(IArticyFlowObject* Node)
{
	auto db = UArticyDatabase::Get(this);
	auto unshadowedObject = db->GetObjectUnshadowed(Cast<UArticyPrimitive>(Node)->GetId());

	// handle pins, because we can not request them directly from the db 
	if (!unshadowedObject)
	{
		auto pinOwner = db->GetObjectUnshadowed(Cast<UArticyFlowPin>(Node)->GetOwner()->GetId());

		TArray<UArticyFlowPin*> pins;
		auto inputPinsOwner = Cast<IArticyInputPinsProvider>(pinOwner);
		pins.Append(*inputPinsOwner->GetInputPins());
		auto outputPinsOwner = Cast<IArticyOutputPinsProvider>(pinOwner);
		pins.Append(*outputPinsOwner->GetOutputPinsPtr());

		auto targetId = Cast<UArticyPrimitive>(Node)->GetId();
		for (auto pin : pins)
		{
			if (pin->GetId() == targetId)
			{
				unshadowedObject = pin;
				break;
			}
		}
	}

	return Cast<IArticyFlowObject>(unshadowedObject);
}

//---------------------------------------------------------------------------//

TArray<FArticyBranch> UArticyFlowPlayer::Explore(IArticyFlowObject* Node, bool bShadowed, int32 Depth)
{
	TArray<FArticyBranch> OutBranches;

	//check stop condition
	if((Depth > ExploreDepthLimit || !Node || (Node != Cursor.GetInterface() && ShouldPauseOn(Node))))
	{
		if(Depth > ExploreDepthLimit)
			UE_LOG(LogArticyRuntime, Warning, TEXT("ExploreDepthLimit (%d) reached, stopping exploration!"), ExploreDepthLimit);
		if(!Node)
			UE_LOG(LogArticyRuntime, Warning, TEXT("Found a nullptr Node when exploring a branch!"));

		/* TODO where should I put this?
		if(OutBranches.Num() >= BranchLimit)
		{
			UE_LOG(LogArticyRuntime, Warning, TEXT("BranchLimit (%d) reached, cannot add another branch!"), BranchLimit);
			return;
		}*/

		//target reached, create a branch
		auto branch = FArticyBranch{};
		if(Node)
		{
			/* NOTE: This check must not be done, as the last node in a branch never affects
			* validity of the branch. A branch is only invalidated if it runs THROUGH a node
			* with invalid condition, instead of just UP TO that node.
			branch.bIsValid = Node->Execute(this); */

			auto unshadowedNode = GetUnshadowedNode(Node);

			TScriptInterface<IArticyFlowObject> ptr;
			ptr.SetObject(unshadowedNode->_getUObject());
			ptr.SetInterface(unshadowedNode);
			branch.Path.Add(ptr);
		}

		OutBranches.Add(branch);
	}
	else
	{
		//set speaker on expresso scripts
		auto xp = GetDB()->GetExpressoInstance();
		if(ensure(xp))
		{
			auto obj = Cast<UArticyPrimitive>(Node);
			if(obj)
			{
				xp->SetCurrentObject(obj);

				IArticyObjectWithSpeaker* speaker;
				if (auto flowPin = Cast<UArticyFlowPin>(Node))
					speaker = Cast<IArticyObjectWithSpeaker>(flowPin->GetOwner());
				else
					speaker = Cast<IArticyObjectWithSpeaker>(obj);

				if(speaker)
					xp->SetSpeaker(speaker->GetSpeaker());
			}
		}

		//if this is the first node, try to submerge
		bool bSubmerged = false;
		if(Depth == 0)
		{
			auto inputPinProvider = Cast<IArticyInputPinsProvider>(Node);
			if(inputPinProvider)
				bSubmerged = inputPinProvider->TrySubmerge(this, OutBranches, Depth + 1, bShadowed); //NOTE: bShadowed will always be true if Depth == 0
		}

		//explore this node
		if(!bSubmerged)
		{
			if(bShadowed)
			{
				//explore the node inside a shadowed operation
				ShadowedOperation([&] { Node->Explore(this, OutBranches, Depth + 1); });
			}
			else
			{
				//non-shadowed explore
				Node->Explore(this, OutBranches, Depth + 1);
			}
		}

		//add this node to the head of all the branches
		for(auto& branch : OutBranches)
		{
			auto unshadowedNode = GetUnshadowedNode(Node);
			TScriptInterface<IArticyFlowObject> ptr;
			ptr.SetObject(unshadowedNode->_getUObject());
			ptr.SetInterface(unshadowedNode);

			branch.Path.Insert(ptr, 0); //TODO inserting at front is not ideal performance wise
		}
	}

	return OutBranches;
}

//---------------------------------------------------------------------------//

void UArticyFlowPlayer::UpdateAvailableBranches()
{
	AvailableBranches.Reset();

	if(PauseOn == 0)
		UE_LOG(LogArticyRuntime, Warning, TEXT("PauseOn is not set, not exploring the Flow as it would not pause on any node."))
	else if(!Cursor)
		UE_LOG(LogArticyRuntime, Warning, TEXT("Cannot explore flow, cursor is not set!"))
	else
	{
		const bool bMustBeShadowed = true;
		AvailableBranches = Explore(&*Cursor, bMustBeShadowed, 0);

		// NP: Every branch needs the index so that Play() can actually take a branch as input
		for (int32 i = 0; i < AvailableBranches.Num(); i++)
			AvailableBranches[i].Index = i;

		//if the cursor is at the StartOn node, check if we should fast-forward
		if(Cursor.GetObject() == StartOn.GetObject(this) && FastForwardToPause())
		{
			//fast-forwarding will call UpdateAvailableBranches again, can abort here
			return;
		}

		//broadcast and return result
		OnPlayerPaused.Broadcast(Cursor);
		OnBranchesUpdated.Broadcast(AvailableBranches);
	}
}

void UArticyFlowPlayer::SetCursorToStartNode()
{
	const auto obj = StartOn.GetObject(this);

	TScriptInterface<IArticyFlowObject> ptr;
	ptr.SetObject(obj);
	ptr.SetInterface(Cast<IArticyFlowObject>(obj));

	SetCursorTo(ptr);
}

bool UArticyFlowPlayer::FastForwardToPause()
{
	//checkNoRecursion(); //this cannot recurse!

	if(AvailableBranches.Num() <= 0)
		return false;

	const auto& firstPath = AvailableBranches[0].Path;
	if(!ensure(firstPath.Num() > 0))
		return false;

	int ffwdIndex;
	for(ffwdIndex = 0; ffwdIndex < firstPath.Num(); ++ffwdIndex)
	{
		const auto node = firstPath[ffwdIndex];
		if(ShouldPauseOn(&*node))
		{
			//pause on this node
			break;
		}

		auto bSplitFound = false;
		for(int b = 1; b < AvailableBranches.Num(); ++b)
		{
			const auto path = AvailableBranches[b].Path;
			//it shouldn't be possible that one path is a subset of the other one
			//(shorter but all nodes equal to the other path)
			if(!ensure(path.IsValidIndex(ffwdIndex)) || path[ffwdIndex] != node)
			{
				bSplitFound = true;
				break;
			}
		}

		if(bSplitFound)
		{
			//pause on the node BEFORE
			--ffwdIndex;
			break;
		}
	}

	if(ffwdIndex <= 0 || ffwdIndex >= firstPath.Num())
	{
		//no need to fast-forward
		return false;
	}

	//create the fast-forward branch
	auto newBranch = FArticyBranch{};
	newBranch.bIsValid = AvailableBranches[0].bIsValid;
	for(int i = 0; i <= ffwdIndex; ++i)
	{
		//add node to branch
		newBranch.Path.Add(firstPath[i]);
	}

	//this also calls UpdateAvailableBranches again
	PlayBranch(newBranch);

	return true;
}

void UArticyFlowPlayer::PlayBranch(const FArticyBranch& Branch)
{
	if(!ensure(ShadowLevel == 0))
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("ArticyFlowPlayer::Traverse was called inside a ShadowedOperation! Aborting Play."))
		return;
	}

	if (!bDisableExecutingOutputPins)
	{
		for (auto node : Branch.Path)
			node->Execute(GetGVs(), GetMethodsProvider());
	}

	bDisableExecutingOutputPins = false;
	Cursor = Branch.Path.Last();
	UpdateAvailableBranches();
}
