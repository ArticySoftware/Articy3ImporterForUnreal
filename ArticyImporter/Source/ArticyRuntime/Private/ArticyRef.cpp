//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "ArticyRef.h"
#include "ArticyObject.h"
#include "ArticyDatabase.h"
#include "ArticyPrimitive.h"

void FArticyRef::SetId(FArticyId NewId)
{
	Id = NewId;
	TechnicalName = "";

	//clear invalid reference, also if it *was not loaded* yet
	if(Reference && (!Reference->WasLoaded() || Reference->GetId()))
		Reference = nullptr;
	//try loading the new reference
	UpdateReference();

	if(Reference)
	{
		TechnicalName = Reference->GetTechnicalName().ToString();
	}
}

void FArticyRef::SetReference(UArticyObject* Object)
{
	if (Object != nullptr && Object->IsValidLowLevelFast())
	{
		Reference = Object;
		Id = Object->GetId();
		TechnicalName = Object->GetTechnicalName().ToString();
	}
	else
	{
		Reference = nullptr;
		TechnicalName = "";
		Id = {};
	}
}

void FArticyRef::UpdateReference()
{
	Reference = UArticyObject::FindAsset(Id);
}

const FArticyId& FArticyRef::GetId() const
{
	//only overwrite the id if the Reference object does exist and was loaded
	if(Reference && Reference->WasLoaded())
		Id = Reference->GetId();
	
	return Id;
}

UArticyObject* FArticyRef::GetReference()//MM_CHANGE
{
	//only overwrite the reference if it is nullptr or *was loaded* but differs from Id
	if (!Reference || (Reference->WasLoaded() && Reference->GetId() != Id))
	{
		Reference = UArticyObject::FindAsset(Id);

		if (TechnicalName.IsEmpty())
		{
			UArticyObject* articyObject = Cast<UArticyObject>(Reference.Get());
			if (articyObject != nullptr)
				TechnicalName = articyObject->GetTechnicalName().ToString();
		}
	}

	return Reference.Get();
}

UArticyObject* FArticyRef::GetObjectInternal(const UObject* WorldContext) const
{
	if(bReferenceBaseObject)
		CloneId = 0;

	auto  database = UArticyDatabase::Get(WorldContext);

	if (!ensureMsgf(database, TEXT("Could not get articy database")))
		return nullptr;


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
