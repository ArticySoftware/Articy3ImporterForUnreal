//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyRef.h"
#include "ArticyObject.h"
#include "ArticyDatabase.h"
#include "ArticyRuntime.h"

void FArticyRef::SetId(FArticyId NewId)
{
	Id = NewId;

	//clear invalid reference, also if it *was not loaded* yet
	if(Reference && (!Reference->WasLoaded() || Reference->GetId()))
		Reference = nullptr;
	//try loading the new reference
	GetReference();
}

void FArticyRef::SetReference(UArticyPrimitive* Object)
{
	Reference = Object;
	SetId(Object ? Object->GetId() : (FArticyId) 0);
}

const FArticyId& FArticyRef::GetId() const
{
	// overwrite the id if the Reference object does exist and was loaded
	if (Reference && Reference->WasLoaded())
	{
		Id = Reference->GetId();
	}
	// if the reference object isn't valid (due to clearing for example), reset the Id too
	else
	{
	//	Id = 0;
	}
	return Id;
}

FArticyId& FArticyRef::GetId()
{
	return Id;
}

UArticyPrimitive* FArticyRef::GetReference()
{
	//only overwrite the reference if it is nullptr or *was loaded* but differs from Id
	if(!Reference || (Reference->WasLoaded() && Reference->GetId() != Id))
		Reference = UArticyPrimitive::FindAsset(Id);

	return Reference;
}

UArticyPrimitive* FArticyRef::GetObjectInternal(const UObject* WorldContext) const
{
	if(bReferenceBaseObject)
		CloneId = 0;

	return UArticyDatabase::Get(WorldContext)->GetOrClone<UArticyPrimitive>(GetId(), CloneId);
}

//bool FArticyRef::Serialize(FArchive& Ar)
//{
//	//we use this as pre-saving hook
//	if(!Ar.IsLoading())
//	{
//		//make sure the Id is up to date before saving
//		Id = GetId();
//	}
//
//	return false; //..so it is still serialized with default serialization
//}
//
//void FArticyRef::PostSerialize(const FArchive& Ar)
//{
//	//we use this as post-loading hook
//#if WITH_EDITOR
//	if(Ar.IsLoading())
//	{
//		GetReference();
//	}
//#endif
//}
