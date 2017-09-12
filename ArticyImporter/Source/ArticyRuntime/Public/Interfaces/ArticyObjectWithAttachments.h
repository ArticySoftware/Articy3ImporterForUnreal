//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyBaseTypes.h"
#include "ArticyObjectWith_Base.h"
#include "ArticyObjectWithAttachments.generated.h"

UINTERFACE(BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithAttachments : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Attachments' implement this interface.
 */
class IArticyObjectWithAttachments : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithAttachments")
	virtual TArray<FArticyId>& GetAttachmentIds()
	{
		static const auto PropName = FName("Attachments");
		return GetProperty<TArray<FArticyId>>(PropName);
	}

	virtual const TArray<FArticyId>& GetAttachmentIds() const
	{
		return const_cast<IArticyObjectWithAttachments*>(this)->GetAttachmentIds();
	}

	/**
	 * Returns an array with a pointer to the referenced attachment objects.
	 * NOTE: References that cannot be resolved (i.e. nullptr) are not contained.
	 */
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithAttachments")
	virtual TArray<UArticyPrimitive*> GetAttachments() const
	{
		TArray<UArticyPrimitive*> attachments;
		
		auto ids = GetAttachmentIds();
		for(const auto& id : ids)
		{
			auto att = id.GetObject(_getUObject());
			if(att)
				attachments.Add(att);
		}
		
		return attachments;
	}

	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithAttachments")
	virtual TArray<FArticyId>& SetAttachmentIds(UPARAM(ref) const TArray<FArticyId>& IDs)
	{
		return GetAttachmentIds() = IDs;
	}

	/**
	 * NOTE: Cannot return a reference to the array stored in the
	 * object, as that one does not exist!
	 */
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithAttachments")
	virtual TArray<UArticyPrimitive*> SetAttachments(UPARAM(ref) const TArray<UArticyPrimitive*>& Objects)
	{
		TArray<FArticyId> IDs;
		for(const auto& obj : Objects)
			IDs.Add(obj->GetId());

		SetAttachmentIds(IDs);

		return Objects;
	}
};
