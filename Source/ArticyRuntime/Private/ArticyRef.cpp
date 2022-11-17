//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyRef.h"
#include "ArticyObject.h"
#include "ArticyDatabase.h"
#include "ArticyPrimitive.h"

void FArticyRef::SetId(FArticyId NewId)
{
	Id = NewId;
}

void FArticyRef::SetReference(UArticyObject* Object)
{
	if (Object != nullptr && Object->IsValidLowLevelFast())
	{
		Id = Object->GetId();
	}
	else
	{
		Id = {};
	}
}

const FArticyId& FArticyRef::GetId() const
{	
	return Id;
}

UArticyObject* FArticyRef::GetObjectInternal(const UObject* WorldContext) const
{
	if(bReferenceBaseObject)
		CloneId = 0;

	auto  database = UArticyDatabase::Get(WorldContext);

	// Changed ensureMsgf to a logged  nullref return to be able  to integrate
	// A flowplayer when building a C++ UActor before importing DB without
	// Throwing an exception to log.
	if(!database)
	{
		UE_LOG(LogTemp,Warning,TEXT("No Articy database found."))
		return nullptr;
	}
		


	return database->GetOrClone<UArticyObject>(GetId(), CloneId);
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
