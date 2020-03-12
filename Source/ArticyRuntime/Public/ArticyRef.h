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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setup")
	bool bReferenceBaseObject = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="!bReferenceBaseObject"), Category="Setup")
	mutable int32 CloneId;

	void SetId(FArticyId NewId);
	void SetReference(UArticyObject* Object);

	const FArticyId& GetId() const;

	/** Get a (single-instance) copy of the referenced asset. */
	template<typename T = UArticyObject>
	T* GetObject(const UObject* WorldContext) const;

	// reference: color.h
	// used in details customization 
	bool InitFromString(const FString& SourceString) const
	{
		Id.High = Id.Low = 0;

		const bool bSuccess = FParse::Value(*SourceString, TEXT("Low="), Id.Low) && FParse::Value(*SourceString, TEXT("High="), Id.High);

		return bSuccess;
	}
private:
	/** The actual reference: we keep track of the Reference's Id.
	 * Set to EditAnywhere here to allow for a hack in manipulating properties
	 * Set to disabled in the ArticyRefCustomization so that the user can't edit it directly
	 */
	UPROPERTY(EditAnywhere, Category="Setup")
	mutable FArticyId Id = 0;

	/** The single-instance object copy of this ArticyRef. */
	mutable TWeakObjectPtr<UArticyObject> CachedObject = nullptr;
	mutable FArticyId CachedId = 0;
	mutable int32 CachedCloneId = 0;

	UArticyObject* GetObjectInternal(const UObject* WorldContext) const;

private:

	////the SerializeOrNot method is our friend!
	//template<class CPPSTRUCT>
	//friend typename TEnableIf<TStructOpsTypeTraits<CPPSTRUCT>::WithSerializer, bool>::Type SerializeOrNot(FArchive& Ar, CPPSTRUCT *Data);

	//bool Serialize(FArchive& Ar);

	////the PostSerializeOrNot method is our friend as well!
	//template<class CPPSTRUCT>
	//friend typename TEnableIf<TStructOpsTypeTraits<CPPSTRUCT>::WithPostSerialize>::Type PostSerializeOrNot(const FArchive& Ar, CPPSTRUCT *Data);

	//void PostSerialize(const FArchive& Ar);
};

template<typename T>
T* FArticyRef::GetObject(const UObject* WorldContext) const
{
	if (!CachedObject.IsValid() || CachedId != Id || CloneId != CachedCloneId)
	{
		CachedId = Id;
		CachedCloneId = CloneId;
		CachedObject = GetObjectInternal(WorldContext);
	}

	return Cast<T>(CachedObject.Get());
}

//template<>
//struct TStructOpsTypeTraits<FArticyRef> : public TStructOpsTypeTraitsBase2<FArticyRef>
//{
//	enum
//	{
//		WithSerializer = true,
//		WithPostSerialize = true
//	};
//};
