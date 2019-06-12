//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyRuntime.h"
#include "ArticyReflectable.h"
#include "AssetRegistryModule.h"
#include "ShadowStateManager.h"
#include "ArticyExpressoScripts.h"

#include "ArticyGlobalVariables.generated.h"

//implicit convertion operator (= getter)
//ReSharper disable once CppNonExplicitConversionOperator
#define ARTICY_VARIABLE_ACCESS(T)			\
	T& operator=(const T &NewValue)			\
	{										\
		/*set and return the new value*/	\
		return Setter<TRemoveReference<decltype(*this)>::Type>(NewValue);		\
	}										\
	const T& Get() const					\
	{										\
		/*just return the value*/			\
		return Value;						\
	}										\
	operator const T &() const				\
	{										\
		/*just return the value*/			\
		return Get();						\
	}
	

class UArticyFlowPlayer;
class UArticyGlobalVariables;
class UArticyVariable;
class UArticyBaseVariableSet;
struct ExpressoType;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGVChanged, UArticyVariable*, Variable);

/**
 * This struct stores a shadow copy that is restored once the given shadow
 * level is dropped.
 */
template<typename Type>
struct ArticyShadowState
{
	ArticyShadowState(const uint32& level, const Type& value) : Level(level), Value(value) { }

	uint32 Level = 0;
	Type Value;
};

USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyGvName
{
	GENERATED_BODY()

public:

	FArticyGvName() = default;
	FArticyGvName(const FName FullVariableName);
	FArticyGvName(const FName VariableNamespace, const FName VariableName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FullName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Namespace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Variable;

	void SetByFullName(const FName FullVariableName);
	void SetByNamespaceAndVariable(const FName VariableNamespace, const FName VariableName);
	const FName& GetNamespace();
	const FName& GetVariable();
	const FName& GetFullName();
};

UCLASS(Abstract, BlueprintType)
class ARTICYRUNTIME_API UArticyVariable : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * This delegate is broadcast every time the (layer zero) value of this variable changes.
	 * A reference to the instance of the variable is passed as parameter.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Callback")
	FOnGVChanged OnVariableChanged;

	template<typename Type>
	void Init(UArticyBaseVariableSet* Set, UArticyGlobalVariables* const NewStore, const FName& Name, const typename Type::UnderlyingType& NewValue);

	/** Returns the name of this variable in the form Namespace.Variable */
	const FName& GetGVName() const { return GVName; }

protected:
	virtual ~UArticyVariable() {}

	//void Init(UArticyBaseGlobalVariables* const NewStore) { this->Store = NewStore; }

	template<typename Type, typename ValueType>
	ValueType& Setter(const ValueType &NewValue)
	{
		auto Instance = static_cast<Type*>(this);
		check(Instance);

		//push the value if we are in a new shadow level now
		const auto storeLevel = GetStoreShadowLevel();
		const auto shadowLevel = GetShadowLevel(Instance);
		if(storeLevel > shadowLevel)
		{																						
			Instance->Shadows.Push(ArticyShadowState<ValueType>{storeLevel, Instance->Value});

			//get notified when the state is popped again
			RegisterOnStorePop(Instance);												
		}								
		else
		{
			//if the global level is different from ShadowLevel, our ShadowLevel is out of sync!
			ensure(storeLevel == shadowLevel);
		}
		
		Instance->Value = NewValue;															
		if(storeLevel == 0)
			OnVariableChanged.Broadcast(this);

		return Instance->Value;
	}																							

	template<typename Type>
	void PopState(Type* Instance)
	{
		if(ensure(GetStoreShadowLevel() == GetShadowLevel(Instance)))
			Instance->Value = Instance->Shadows.Pop().Value;
	}

	template<typename Type>
	static uint32 GetShadowLevel(Type* Instance);
	uint32 GetStoreShadowLevel() const;

	/** The name of this variable in the form Namespace.Variable */
	UPROPERTY(BlueprintReadOnly)
	FName GVName;

private:

	UPROPERTY()
	UArticyGlobalVariables* Store = nullptr;

	template<typename Type>
	void RegisterOnStorePop(Type* Instance);
};

//---------------------------------------------------------------------------//

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyInt : public UArticyVariable
{
	GENERATED_BODY()

public:
	typedef int UnderlyingType;

	friend UArticyVariable;

public:
	//void Init(UArticyBaseGlobalVariables* const NewStore, const int& NewValue) { UArticyVariable::Init(NewStore); Set(NewValue); }

	//getter and setter
	ARTICY_VARIABLE_ACCESS(int)

	//other operators
	int& operator+=(const int &Val) { return *this = Value + Val; }
	int& operator-=(const int &Val) { return *this = Value - Val; }
	int& operator*=(const int &Val) { return *this = Value * Val; }
	int& operator/=(const int &Val) { return *this = Value / Val; }

	int operator++(int)
	{
		int copy = *this;
		*this = Value + 1;
		return copy;
	}
	int& operator++() { return *this = Value + 1; }

	int operator--(int)
	{
		int copy = *this;
		*this = Value - 1;
		return copy;
	}
	int& operator--() { return *this = Value + 1; }

	int& operator=(const ExpressoType &NewVal)
	{
		if (NewVal.Type == ExpressoType::Float)
			return Value = NewVal.GetFloat();
		else
			return Value = NewVal.GetInt();
	}

	int& operator+=(const ExpressoType &Val)
	{
		if (Val.Type == ExpressoType::Float)
			return *this = Value + Val.GetFloat();
		else
			return *this = Value + Val.GetInt();
	}

	int& operator-=(const ExpressoType &Val)
	{
		if (Val.Type == ExpressoType::Float)
			return *this = Value - Val.GetFloat();
		else
			return *this = Value - Val.GetInt();
	}

	int& operator*=(const ExpressoType &Val)
	{
		if (Val.Type == ExpressoType::Float)
			return *this = Value * Val.GetFloat();
		else
			return *this = Value * Val.GetInt();
	}

	int& operator/=(const ExpressoType &Val)
	{
		if (Val.Type == ExpressoType::Float)
			return *this = Value / Val.GetFloat();
		else
			return *this = Value / Val.GetInt();
	}

protected:
	/** The current value of this variable (i.e. the value of a shadow state, if any is active). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int Value = -1;

	/**
	 * Set the value of this variable.
	 * This internally calls the setter, so it guarantees that the correct shadow state is used (if any).
	 */
	UFUNCTION(BlueprintCallable, Category="ValueAccess")
	int Set(int NewValue) { return *this = NewValue; }

private:
	TArray<ArticyShadowState<int>> Shadows;
};

//---------------------------------------------------------------------------//

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyBool : public UArticyVariable
{
	GENERATED_BODY()

public:
	typedef bool UnderlyingType;

	friend UArticyVariable;

	//void Init(UArticyBaseGlobalVariables* const NewStore, const bool& NewValue) { UArticyVariable::Init(NewStore); Set(NewValue); }

	//getter and setter
	ARTICY_VARIABLE_ACCESS(bool)

	bool& operator=(const ExpressoType &NewValue)
	{
		return Value = NewValue.GetBool();
	}

protected:
	/** The current value of this variable (i.e. the value of a shadow state, if any is active). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Value = false;

	/**
	 * Set the value of this variable.
	 * This internally calls the setter, so it guarantees that the correct shadow state is used (if any).
	 */
	UFUNCTION(BlueprintCallable, Category="ValueAccess")
	bool Set(bool NewValue) { return *this = NewValue; }

private:
	TArray<ArticyShadowState<bool>> Shadows;
};

//---------------------------------------------------------------------------//

UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyString : public UArticyVariable
{
	GENERATED_BODY()

public:
	typedef FString UnderlyingType;

	friend UArticyVariable;

	//void Init(UArticyBaseGlobalVariables* const NewStore, const FString& NewValue) { UArticyVariable::Init(NewStore); Set(NewValue); }

	//getter and setter
	ARTICY_VARIABLE_ACCESS(FString)

	//other operators
	//FString& operator+=(const FString &Val) { return Setter<UArticyString>(Value + Val); }

	FString& operator+=(const ExpressoType &Val) { return Setter<UArticyString>(Value + Val.GetString()); }

	FString& operator=(const ExpressoType &NewValue)
	{
		if (NewValue.Type == ExpressoType::Int) // used to store a string representation of an articy object
			return Value = ArticyHelpers::Uint64ToObjectString(NewValue.GetInt());
		else
			return Value = NewValue.GetString();
	}

protected:
	/** The current value of this variable (i.e. the value of a shadow state, if any is active). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Value;

	/**
	 * Set the value of this variable.
	 * This internally calls the setter, so it guarantees that the correct shadow state is used (if any).
	 */
	UFUNCTION(BlueprintCallable, Category="ValueAccess")
	FString Set(FString NewValue) { return *this = NewValue; }

private:
	TArray<ArticyShadowState<FString>> Shadows;
};

//---------------------------------------------------------------------------//

UCLASS()
class ARTICYRUNTIME_API UArticyBaseVariableSet : public UObject, public IArticyReflectable
{
	GENERATED_BODY()

public:
	/**
	 * This delegate is broadcast every time a variable inside this namespace changes.
	 * A reference to the instance of the changed variable is passed as parameter.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Callback")
	FOnGVChanged OnVariableChanged;

	UFUNCTION(BlueprintCallable, Category = "Variables")
	const TArray<UArticyVariable*> GetVariables() const { return Variables; }

protected:

	UPROPERTY()
	TArray<UArticyVariable*> Variables;

private:

	UFUNCTION()
	void BroadcastOnVariableChanged(UArticyVariable* Variable);

	template <typename Type>
	friend void UArticyVariable::Init(UArticyBaseVariableSet* Set, UArticyGlobalVariables* const NewStore, const FName& Name, const typename Type::UnderlyingType& NewValue);
};

/**
 * The base class for the (generated) ArticyGlobalVariables class.
 */
UCLASS()
class ARTICYRUNTIME_API UArticyGlobalVariables : public UDataAsset, public IShadowStateManager, public IArticyReflectable
{
	GENERATED_BODY()	

public:

	/**
	 * Get a reference to the ArticyGlobalVariables asset.
	 * Creates a temporary copy if the 
	 */
	static UArticyGlobalVariables* GetDefault(const UObject* WorldContext);

	/* Unloads the global variables, which causes that all changes get removed. */
	UFUNCTION(BlueprintCallable, Category = "Packages")
	void UnloadGlobalVariables();

	UFUNCTION(BlueprintCallable, Category="Getter")
	UArticyBaseVariableSet* GetNamespace(const FName Namespace);

	UFUNCTION(BlueprintCallable, Category = "Getter")
	const TArray<UArticyBaseVariableSet*> GetVariableSets() const { return VariableSets; }
	
	UFUNCTION(BlueprintCallable, Category = "Getter")
	const bool& GetBoolVariable(FArticyGvName GvName, bool& bSucceeded);
	UFUNCTION(BlueprintCallable, Category="Getter")
	const int32& GetIntVariable(FArticyGvName GvName, bool& bSucceeded);
	UFUNCTION(BlueprintCallable, Category="Getter")
	const FString& GetStringVariable(FArticyGvName GvName, bool& bSucceeded);

	UFUNCTION(BlueprintCallable, Category="Setter")
	void SetBoolVariable(FArticyGvName GvName, const bool Value);
	UFUNCTION(BlueprintCallable, Category="Setter")
	void SetIntVariable(FArticyGvName GvName, const int32 Value);
	UFUNCTION(BlueprintCallable, Category="Setter")
	void SetStringVariable(FArticyGvName GvName, const FString Value);

protected:

	UPROPERTY()
	TArray<UArticyBaseVariableSet*> VariableSets;

private:

	static TWeakObjectPtr<UArticyGlobalVariables> Clone;

	template <typename ArticyVariableType, typename VariablePayloadType>
	void SetVariableValue(const FName Namespace, const FName Variable, const VariablePayloadType Value);
	template <typename ArticyVariableType, typename VariablePayloadType>
	void SetVariableValue(const FName FullVariableName, const VariablePayloadType Value);

	template<typename ArticyVariableType, typename VariablePayloadType>
	const VariablePayloadType& GetVariableValue(const FName Namespace, const FName Variable, bool& bSucceeded);
	template<typename ArticyVariableType, typename VariablePayloadType>
	const VariablePayloadType& GetVariableValue(const FName FullVariableName, bool& bSucceeded);
};

//---------------------------------------------------------------------------//
// TEMPLATED METHODS
//---------------------------------------------------------------------------//

template <typename Type>
void UArticyVariable::Init(UArticyBaseVariableSet* Set, UArticyGlobalVariables* const NewStore, const FName& Name, const typename Type::UnderlyingType& NewValue)
{
	GVName = Name;
	Store = NewStore;

	//remove all listeners (there should not be any)
	OnVariableChanged.Clear();
	
	//set the initial value
	Setter<Type>(NewValue);

	//register the set's OnVariableChanged delegate on the variable's
	OnVariableChanged.AddDynamic(Set, &UArticyBaseVariableSet::BroadcastOnVariableChanged);
}

template <typename Type>
uint32 UArticyVariable::GetShadowLevel(Type* Instance)
{
	return Instance->Shadows.Num() > 0 ? Instance->Shadows.Last().Level : 0;
}

template <typename Type>
void UArticyVariable::RegisterOnStorePop(Type* Instance)
{
	Store->RegisterOnPopState([=] { this->PopState(Instance); });
}

template <typename ArticyVariableType, typename VariablePayloadType>
void UArticyGlobalVariables::SetVariableValue(const FName Namespace, const FName Variable, const VariablePayloadType Value)
{
	auto set = GetNamespace(Namespace);
	if (set)
	{
		auto ptr = set->GetPropPtr<ArticyVariableType *>(Variable);
		if (ptr)
		{
			auto& propValue = (**ptr);
			propValue = Value;
		}
	}
}

template<typename ArticyVariableType, typename VariablePayloadType>
const VariablePayloadType& UArticyGlobalVariables::GetVariableValue(const FName Namespace, const FName Variable, bool& bSucceeded)
{
	auto set = GetNamespace(Namespace);
	if (set)
	{
		auto ptr = set->GetPropPtr<ArticyVariableType *>(Variable);
		if (ptr)
		{
			auto& propValue = (**ptr);
			bSucceeded = true;
			return propValue.Get();
		}
	}

	bSucceeded = false;
	static VariablePayloadType empty = VariablePayloadType();
	return empty;
}
