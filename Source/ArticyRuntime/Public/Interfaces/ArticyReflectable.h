//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "Launch/Resources/Version.h"
#include "ArticyReflectable.generated.h"

// Restore deprecation message for anyone trying to use UProperty after this file.
// This only applies to 4.25 because that's the version that had both FProperty and UProperty supported (afterwards, only FProperty)
//  Once we no longer need to support <4.25, we can just replace all UProperty's with FProperty's and delete all related #defines
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 25
#undef FProperty
#define FProperty DEPRECATED_MACRO(4.25, "UProperty has been renamed to FProperty") UProperty
#endif

UINTERFACE()
class UArticyReflectable : public UInterface { GENERATED_BODY() };

/**
 * This interface provides convinience methods to get and set properties via reflection.
 * It also caches reflection information on a per (runtime-)class basis, so it is faster
 * than directly using the unreal reflection system.
 */
class IArticyReflectable
{
	GENERATED_BODY()

public:
	/** Set the property with a given name. */
	template<typename TValue>
	TValue SetProp(FName Property, TValue Value, int32 ArrayIndex = 0);

	/** Get the property with a given name. */
	template<typename TValue>
	TValue& GetProp(FName Property, int32 ArrayIndex = 0);

	/** Get the const property with a given name. */
	template<typename TValue>
	const TValue& GetProp(FName Property, int32 ArrayIndex = 0) const
	{
		//the const cast here is only used so we don't have to re-implement the same method again
		return const_cast<IArticyReflectable*>(this)->GetProp<TValue>(Property, ArrayIndex);
	}

	/** Returns the pointer to a property of a given name. */
	template<typename TValue>
	TValue* GetPropPtr(FName Property, int32 ArrayIndex = 0) const
	{
		//look up the (hopefully already cached) property pointers
		auto propPointers = GetPropertyPointers();
		auto prop = propPointers.Find(Property);
		if(prop)
			return (*prop)->ContainerPtrToValuePtr<TValue>(_getUObject(), ArrayIndex);

		return nullptr;
	}

	/** Returns the pointer to a property of a given name. */
	FProperty* GetProperty(FName Property) const
	{
		//look up the (hopefully already cached) property pointers
		auto propPointers = GetPropertyPointers();
		auto prop = propPointers.Find(Property);
		if(prop)
			return *prop;

		return nullptr;
	}

	/** Returns true if the Property can be found on the given Class. */
	static bool HasProperty(const UClass* Class, const FName &Property)
	{
		return GetPropertyPointers(Class).Find(Property) != nullptr;
	}

	virtual UClass* GetObjectClass() const { return _getUObject()->GetClass(); }

private:
	/**
	 * Caches and returns all UProperty pointers of this UClass.
	 * They are cached because they can only be found by iterating over
	 * them, using the TFieldIterator.
	 */
	TMap<FName, FProperty*>& GetPropertyPointers() const
	{
		return GetPropertyPointers(GetObjectClass());
	}

	static TMap<FName, FProperty*>& GetPropertyPointers(const UClass* Class)
	{
		static TMap<const UClass*, TMap<FName, FProperty*>> PropertyPointers;
		auto& pp = PropertyPointers.FindOrAdd(Class);
		if(pp.Num() == 0)
		{
			//cache property pointers
			for(TFieldIterator<FProperty> It(Class); It; ++It)
				pp.Add(*It->GetNameCPP(), *It);
		}

		return pp;
	}
};

//---------------------------------------------------------------------------//

template <typename TValue>
TValue IArticyReflectable::SetProp(FName Property, TValue Value, int32 ArrayIndex)
{
	TValue* valPtr = GetPropPtr<TValue>(Property, ArrayIndex);
	if(valPtr)
	{
		(*valPtr) = Value;
		return (*valPtr);
	}

	return Value;
}

template <typename TValue>
TValue& IArticyReflectable::GetProp(FName Property, int32 ArrayIndex)
{
	TValue* valPtr = GetPropPtr<TValue>(Property, ArrayIndex);
	if(valPtr)
		return (*valPtr);

	static TValue Empty;
	return Empty;
}

// Restore deprecation message for anyone trying to use UProperty after this file.
// This only applies to 4.25 because that's the version that had both FProperty and UProperty supported (afterwards, only FProperty)
//  Once we no longer need to support <4.25, we can just replace all UProperty's with FProperty's and delete all related #defines
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 25
#undef FProperty
#define FProperty DEPRECATED_MACRO(4.25, "UProperty has been renamed to FProperty") UProperty
#endif