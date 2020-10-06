//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "Launch/Resources/Version.h"
#include "ArticyReflectable.generated.h"

// #TODO Remove this and restore at the bottom in the future
#if ENGINE_MINOR_VERSION == 25
#ifdef UProperty
	#undef UProperty
	#define UProperty FProperty
#endif
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
	UProperty* GetProperty(FName Property) const
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

	virtual UClass* GetClass() const { return _getUObject()->GetClass(); }

private:
	/**
	 * Caches and returns all UProperty pointers of this UClass.
	 * They are cached because they can only be found by iterating over
	 * them, using the TFieldIterator.
	 */
	TMap<FName, UProperty*>& GetPropertyPointers() const
	{
		return GetPropertyPointers(GetClass());
	}

	static TMap<FName, UProperty*>& GetPropertyPointers(const UClass* Class)
	{
		static TMap<const UClass*, TMap<FName, UProperty*>> PropertyPointers;

		auto& pp = PropertyPointers.FindOrAdd(Class);
		if(pp.Num() == 0)
		{
			//cache property pointers
			for(TFieldIterator<UProperty> It(Class); It; ++It)
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

#if ENGINE_MINOR_VERSION == 25
#undef UProperty
#define UProperty DEPRECATED_MACRO(4.25, "UProperty has been renamed to FProperty") FProperty
#endif
