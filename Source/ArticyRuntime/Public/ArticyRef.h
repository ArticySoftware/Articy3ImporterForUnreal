//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyBaseTypes.h"
#include "ArticyObject.h"
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

	/** The currently assigned clone ID. Use */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition="!bReferenceBaseObject"), Category="Setup")
	mutable int32 CloneId = 0;

	void SetId(FArticyId NewId);
	void SetReference(UArticyObject* Object);

	const FArticyId& GetId() const;

	/** Get a (single-instance) copy of the referenced asset. */
	template<typename T = UArticyObject>
	T* GetObject(const UObject* WorldContext) const;

	uint32 GetEffectiveCloneId() const
	{
		return bReferenceBaseObject ? 0ul : *reinterpret_cast<uint32*>(&CloneId);
	}
	
	enum EStringInitResult
	{
		NoneSet,
		IdSet,
		RefSet,
		AllSet
	};

	// reference: color.h
	// used in details customization
	EStringInitResult InitFromString(const FString& SourceString)
	{
		Id.High = Id.Low = 0;

		const bool IdSet =
			FParse::Value(*SourceString, TEXT("Low="), Id.Low) &&
			FParse::Value(*SourceString, TEXT("High="), Id.High);
		const bool RefSet =
			FParse::Bool(*SourceString, TEXT("bReferenceBaseObject="), bReferenceBaseObject) &&
			FParse::Value(*SourceString, TEXT("CloneId="), CloneId);

		return IdSet && RefSet ? EStringInitResult::AllSet : IdSet ? EStringInitResult::IdSet : RefSet ? EStringInitResult::RefSet : EStringInitResult::NoneSet;
	}

	/** TODO
	 * The ToString function is used to update Refs in the property system.
	 * When determining if a ref is the same as another ref in a TMap in BP, the ExportItem function is used and strings are compared
	 * The ExportItem function delivers Strings like below, but without spaces. Therefore it is possible to insert 2x the same ArticyRef.
	 * However, when we remove the spaces, typical BP Map problems start existing.
	 * If CharacterA Base = true exists, you can't add another CharacterA (because Base would be initialized to true)
	 * Keep it like it is for now, as compiling will remove the duplicates.
	 *
	 * Future fixes: When Base is set to true, just set CloneId to 0 (data loss)
	 * Or use another ToString function that only works with effective data, not literal data!
	 * Same goes for FArticyId!
	 */
	FString ToString() const
	{
		return FString::Printf(TEXT("(bReferenceBaseObject=%s, CloneId=%d, Id=(Low=%d, High=%d))"), bReferenceBaseObject ? TEXT("True") : TEXT("False"), CloneId, Id.Low, Id.High);
	}

	/** Is used in hash operations, and in that case we want to know about effective data rather than literal data,
	 *  so a 'hidden' CloneID (bReferenceBaseObject = true) does not influence the comparison
	 */
	bool operator==(const FArticyRef& Other) const
	{
		return MatchesEffective(Other);
	}

	/** Used to compare raw data; CloneID might have different values between two ArticyRefs that have bReferenceBaseObject = true. */
    bool MatchesRaw(const FArticyRef& Other) const
	{
		return Other.Id.Low == Id.Low && Other.Id.High == Id.High && Other.bReferenceBaseObject == bReferenceBaseObject && Other.CloneId == CloneId;
	}

	/** Used to compare effective data; will act as if CloneID was 0 when bReferenceBaseObject = true.*/
    bool MatchesEffective(const FArticyRef& Other) const
	{
		return Other.Id.Low == Id.Low && Other.Id.High == Id.High && Other.bReferenceBaseObject == bReferenceBaseObject && Other.GetEffectiveCloneId() == GetEffectiveCloneId();
	}

	bool operator!=(const FArticyRef& Other) const
	{
		return !this->operator==(Other);
	}

	friend uint32 GetTypeHash(const FArticyRef& Ref)
	{
		return HashCombine(GetTypeHash(Ref.GetId()), Ref.GetEffectiveCloneId());
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
