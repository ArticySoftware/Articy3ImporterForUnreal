//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyBaseTypes.h"

#include "ArticyRef.generated.h"

/**
*
*/
USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyRef
{
	GENERATED_BODY()

public:
	/** The original object corresponding to the Id. */
	UPROPERTY(EditAnywhere, Category="Setup")
	UArticyPrimitive* Reference = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	bool bReferenceBaseObject = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="!bReferenceBaseObject"), Category="Setup")
	mutable int32 CloneId;

	void SetId(FArticyId NewId);
	void SetReference(UArticyPrimitive* Object);

	const FArticyId& GetId() const;
	UArticyPrimitive* GetReference();

	/** Get a (single-instance) copy of the referenced asset. */
	template<typename T = UArticyObject>
	T* GetObject(const UObject* WorldContext) const;

private:
	/** The actual reference: we keep track of the Reference's Id. */
	UPROPERTY(VisibleAnywhere, Category="Setup")
	mutable FArticyId Id = 0;

	/** The single-instance object copy of this ArticyRef. */
	mutable TWeakObjectPtr<UArticyPrimitive> CachedObject = nullptr;
	mutable FArticyId CachedId = 0;
	mutable int32 CachedCloneId = 0;

	UArticyPrimitive* GetObjectInternal(const UObject* WorldContext) const;

private:

	//the SerializeOrNot method is our friend!
	template<class CPPSTRUCT>
	friend typename TEnableIf<TStructOpsTypeTraits<CPPSTRUCT>::WithSerializer, bool>::Type SerializeOrNot(FArchive& Ar, CPPSTRUCT *Data);

	bool Serialize(FArchive& Ar);

	//the PostSerializeOrNot method is our friend as well!
	template<class CPPSTRUCT>
	friend typename TEnableIf<TStructOpsTypeTraits<CPPSTRUCT>::WithPostSerialize>::Type PostSerializeOrNot(const FArchive& Ar, CPPSTRUCT *Data);

	void PostSerialize(const FArchive& Ar);
};

template<typename T>
T* FArticyRef::GetObject(const UObject* WorldContext) const
{
	return Cast<T>(GetObject<UArticyPrimitive>(WorldContext));
}

template<>
inline UArticyPrimitive* FArticyRef::GetObject(const UObject* WorldContext) const
{
	if (!CachedObject.IsValid() || CachedId != Id || CloneId != CachedCloneId)
	{
		CachedId = Id;
		CachedCloneId = CloneId;
		CachedObject = GetObjectInternal(WorldContext);
	}

	return CachedObject.Get();
}

template<>
struct TStructOpsTypeTraits<FArticyRef> : public TStructOpsTypeTraitsBase2<FArticyRef>
{
	enum
	{
		WithSerializer = true,
		WithPostSerialize = true
	};
};
