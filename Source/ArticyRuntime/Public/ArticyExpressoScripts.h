//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "Internationalization/Regex.h"
#include "ArticyObject.h"
#include "ArticyDatabase.h"

#include "ArticyExpressoScripts.generated.h"

class UArticyString;
class UArticyInt;
class UArticyBool;
class UArticyExpressoScripts;
struct ExpressoType;

struct ARTICYRUNTIME_API ExpressoType
{
    union
    {
        bool BoolValue;
        int64 IntValue = 0;
        double FloatValue;
    };
    FString StringValue;

    enum EType
    {
        Undefined, Bool, Int, Float, String
    } Type = Undefined;

    virtual bool& GetBool();
    virtual const bool& GetBool() const;

    virtual int64& GetInt();
    virtual const int64& GetInt() const;

    virtual double& GetFloat();
    virtual const double& GetFloat() const;

    virtual FString& GetString();
    virtual const FString& GetString() const;

    virtual FString ToString() const;


    //---------------------------------------------------------------------------//

    ExpressoType();
    virtual ~ExpressoType();

    //initialize from object and property
    ExpressoType(UArticyBaseObject* Object, const FString& Property);
    
    // ReSharper disable CppNonExplicitConvertingConstructor

    //implicit conversion from value type
    ExpressoType(const bool& Value);
    ExpressoType(const int64& Value);
    ExpressoType(const int32& Value);
    ExpressoType(const int16& Value);
    ExpressoType(const int8& Value);
    ExpressoType(const uint64& Value);
    ExpressoType(const uint32& Value);
    ExpressoType(const uint16& Value);
    ExpressoType(const uint8& Value);
    ExpressoType(const double& Value);
    ExpressoType(const float& Value);
    ExpressoType(const FString& Value);
    ExpressoType(const FText& Value);
    ExpressoType(const FName& Value);
    ExpressoType(const UArticyPrimitive* Object);
    ExpressoType(const UArticyString& Value);
    ExpressoType(const UArticyInt& Value);
    ExpressoType(const UArticyBool& Value);
    ExpressoType(const FArticyId& Value);
    
    //implicit conversion to value type
    explicit operator bool() const;
    explicit operator int64() const;
    explicit operator int8() const;
    explicit operator uint8() const;
    explicit operator int16() const;
    explicit operator uint16() const;
    explicit operator int32() const;
    explicit operator uint32() const;
    explicit operator uint64() const;
    explicit operator double() const;
    explicit operator float() const;
    explicit operator FString() const;
    explicit operator FText() const;
    explicit operator FName() const;
    explicit operator FArticyId() const;

    // ReSharper restore CppNonExplicitConvertingConstructor

    //---------------------------------------------------------------------------//

    ExpressoType operator-() const;

    bool operator==(const ExpressoType& Other) const;
    bool operator!=(const ExpressoType& Other) const;
    bool operator<(const ExpressoType& Other) const;
    bool operator>(const ExpressoType& Other) const;
    bool operator<=(const ExpressoType& Other) const;
    bool operator>=(const ExpressoType& Other) const;

    ExpressoType operator&&(const ExpressoType& Other) const;
    ExpressoType operator||(const ExpressoType& Other) const;
    ExpressoType operator^(const ExpressoType& Other) const;

    ExpressoType operator+(const ExpressoType& Other) const;
    ExpressoType operator-(const ExpressoType& Other) const;
    ExpressoType operator*(const ExpressoType& Other) const;
    ExpressoType operator/(const ExpressoType& Other) const;
    ExpressoType operator%(const ExpressoType& Other) const;

    //NOTE: No assignment/mutation operators are defined, as those are not
    //allowed anywhere where a ExpressoType is used!
    /*ExpressoType& operator++();
    ExpressoType operator++(int);
    ExpressoType& operator--();
    ExpressoType operator--(int);*/

    //---------------------------------------------------------------------------//

    struct Definition;
    
    /**
     * This map contains a Factory and a Setter lambda for all the types
     * that may exist as properties. This is needed because we need the
     * cpp type (template argument) for reflection.
     */
    static TMap<FName, Definition> Definitions;

    const Definition& GetDefinition(const FName& CppType) const;

    template<typename T>
    static void AddDefinition(const FName& CppType);
    
    //---------------------------------------------------------------------------//

    /** Set the property with a given name, without knowing the type. */
    void SetValue(UArticyBaseObject* Object, FString Property) const;

    /** If Property contains a dot, this method modifies Object and Property so a fueature is accessed instead. */
    static UArticyBaseObject* TryFeatureReroute(UArticyBaseObject* Object, FString& Property);
};

struct ExpressoType::Definition
{
    //function to create an ArticyExpressoType from a FProperty
    TFunction<ExpressoType(UArticyBaseObject*, FProperty*)> Factory;
    //function to set a uproperty from an ArticyExpressoType value
    TFunction<void(UArticyBaseObject*, FProperty*, const ExpressoType&)> Setter;
};

template<typename T>
void ExpressoType::AddDefinition(const FName& CppType)
{
    Definition def;

    def.Factory = [](UArticyBaseObject* Object, FProperty* Property)
        {
            if (Object && Property)
            {
                T* ptr = Property->ContainerPtrToValuePtr<T>(Object);
                if (ptr)
                    return ExpressoType(*ptr);
            }

            return ExpressoType{};
        };

    def.Setter = [](UArticyBaseObject* Object, FProperty* Property, const ExpressoType& Value)
        {
            if (!Object || !Property)
                return;

            T* ptr = Property->ContainerPtrToValuePtr<T>(Object);
            if (ptr)
                (*ptr) = (T)Value;
        };

    Definitions.Add(CppType, def);
}

FORCEINLINE int operator+(const int& Lhs, const ExpressoType& Rhs)
{
	return Lhs + (int)Rhs;
}

FORCEINLINE int operator-(const int& Lhs, const ExpressoType& Rhs)
{
	return Lhs - (int)Rhs;
}

FORCEINLINE int operator*(const int& Lhs, const ExpressoType& Rhs)
{
	return Lhs * (int)Rhs;
}

FORCEINLINE int operator/(const int& Lhs, const ExpressoType& Rhs)
{
	return Lhs / (int)Rhs;
}

FORCEINLINE int operator%(const int& Lhs, const ExpressoType& Rhs)
{
	return Lhs % (int)Rhs;
}

FORCEINLINE float operator+(const float& Lhs, const ExpressoType& Rhs)
{
	return Lhs + (float)Rhs;
}

FORCEINLINE float operator-(const float& Lhs, const ExpressoType& Rhs)
{
	return Lhs - (float)Rhs;
}

FORCEINLINE float operator*(const float& Lhs, const ExpressoType& Rhs)
{
	return Lhs * (float)Rhs;
}

FORCEINLINE float operator/(const float& Lhs, const ExpressoType& Rhs)
{
	return Lhs / (float)Rhs;
}

/**
 * The Expresso Scripts class is keeping track of conditions & instructions via hashes and manages parameters
 */
UCLASS(abstract)
class ARTICYRUNTIME_API UArticyExpressoScripts : public UObject
{
	GENERATED_BODY()

public:

	UArticyExpressoScripts()
	{
		//add empty condition and instruction
		Conditions.Add(GetTypeHash(FString{ "" }), [&] { return true; });
		Instructions.Add(GetTypeHash(FString{ "" }), [&] { return; });
	}

	/** Used by the FlowPlayer internally. */
	virtual UClass* GetUserMethodsProviderInterface() { return nullptr; }

	virtual void Init(UArticyDatabase* DB) { OwningDatabase = DB; }
	UArticyDatabase* GetDb() const { return OwningDatabase; }


	void SetCurrentObject(UArticyPrimitive* Object) { self = Object; }
	void SetSpeaker(UArticyObject* Speaker) { speaker = Speaker; }

	/**
	 * Evaluate the condition and return the result.
	 * Note that the passed in condition is only used as a key to look up a lambda in a map of existing (imported) conditions!
	 */
	bool Evaluate(const int &ConditionFragmentHash, UArticyGlobalVariables* GV, UObject* MethodProvider) const;
	/**
	 * Execute the instruction, and return true (unless the fragment was not found!)
	 * Note that the passed in condition is only used as a key to look up a lambda in a map of existing (imported) conditions!
	 */
	bool Execute(const int &InstructionFragmentHash, UArticyGlobalVariables* GV, UObject* MethodProvider) const;

	/**
	 * Sets a default method provider, which will be always used whenever scripts get
	 * evaluated / executed without a valid method provider.
	 */
	void SetDefaultUserMethodsProvider(UObject* MethodProvider);
	UObject* GetDefaultUserMethodsProvider() const;
	UObject* GetUserMethodsProviderObject() const;

	/** Gets the active GV instance in use if we're running an expresso script (otherwise nullptr) */
	virtual UArticyGlobalVariables* GetGV() { return nullptr; }

protected:

	/** Sets the GV instance to be used when executing expresso script fragments. */
	virtual void SetGV(UArticyGlobalVariables* GV) const { }

	//========================================//

	/**
	 * The current object where the script is evaluated on.
	 * Don't change the name, it's called like this in script fragments!
	 */
	UArticyPrimitive* self = nullptr;
	/**
	 * Inside a DialogFragment this is a reference to the current speaker.
	 * Don't change the name, it's called like this in script fragments!
	 */
	UArticyObject* speaker = nullptr;

	TMap<uint32, TFunction<bool()>> Conditions;
	TMap<uint32, TFunction<void()>> Instructions;

	/** Don't change the name, it's called like this in script fragments! */
	UArticyObject* getObj(const FString& NameOrId, const uint32& CloneId = 0) const;

	/** Don't change the name, it's called like this in script fragments! */
	static void setProp(UArticyBaseObject* Object, const FString& Property, const ExpressoType& Value);
	/** Don't change the name, it's called like this in script fragments! */
	void setProp(const ExpressoType& Id_CloneId, const FString& Property, const ExpressoType& Value) const;

	/** Don't change the name, it's called like this in script fragments! */
	static ExpressoType getProp(UArticyBaseObject* Object, const FString& Property);
	/** Don't change the name, it's called like this in script fragments! */
	ExpressoType getProp(const ExpressoType& Id_CloneId, const FString& Property) const;

	/** Don't change the name, it's called like this in script fragments! */
	int random(int Min, int Max);
	/** Don't change the name, it's called like this in script fragments! */
	int random(int Max);
	/** Don't change the name, it's called like this in script fragments! */
	float random(float Min, float Max);
	/** Don't change the name, it's called like this in script fragments! */
	float random(float Max);

	/** Don't change the name, it's called like this in script fragments! */
	ExpressoType random(const ExpressoType& Min, const ExpressoType& Max);
	ExpressoType random(const ExpressoType& Max);

	/** Don't change the name, it's called like this in script fragments! */
	static void incrementProp(UArticyBaseObject* Object, const FString& Property, const float Value = 1);
	/** Don't change the name, it's called like this in script fragments! */
	void incrementProp(const ExpressoType& Id_CloneId, const FString& Property, const float Value = 1) const;

	/** Don't change the name, it's called like this in script fragments! */
	static void decrementProp(UArticyBaseObject* Object, const FString& Property, const float Value = 1);
	/** Don't change the name, it's called like this in script fragments! */
	void decrementProp(const ExpressoType& Id_CloneId, const FString& Property, const float Value = 1) const;

	/** Don't change the names of the following methods as it's mapped as it from script fragments! */
	static bool isInRange(float valueToTest, float lowerBound, float upperBound);
	static bool isPropInRange(UArticyBaseObject* Object, const FString& Property, float lowerBound, float upperBound);
	bool isPropInRange(const ExpressoType& Id_CloneId, const FString& Property, float lowerBound, float upperBound) const;

	static bool isInRange(const FString& valueToTest, const FString& lowerBound, const FString& upperBound);
	static bool isPropInRange(UArticyBaseObject* Object, const FString& Property, const FString& lowerBound, const FString& upperBound);
	bool isPropInRange(const ExpressoType& Id_CloneId, const FString& Property, const FString& lowerBound, const FString& upperBound) const;

	/**
	 * Prints a string to the log. Can contain placeholders:
	 * print("{0} contain a placeholder!", "I")
	 * Don't change the name, it's called like this in script fragments!
	 */
	template<typename ...ArgTypes>
	static void print(const FString& Msg, ArgTypes... Args);
	/**
	 * Prints a string to the log. Can contain placeholders:
	 * print("{0} contain a placeholder!", "I")
	 * Don't change the name, it's called like this in script fragments!
	 */
	template<typename ...ArgTypes>
	static void print(const ExpressoType& Msg, ArgTypes... Args)
	{
		print(Msg.ToString(), Args...);
	}

	/** Script conditions that are not empty, but rather contain something that evaluates to bool, return that condition. */
	static const bool& ConditionOrTrue(const bool &Condition) { return Condition; }
	/** Script conditions that are empty or only contain a comment always return true. */
	static bool ConditionOrTrue(void /*JustAComment*/) { return true; }

	// This is a cache of the current methods provider set during Evaluate to use while running instructions/conditions
	mutable UObject* UserMethodsProvider = nullptr;

	// Default Methods Provider (fallback if none is set). This is a weak pointer in case it is deleted.
	TWeakObjectPtr<UObject> DefaultUserMethodsProvider = nullptr;

private:
	
	UArticyDatabase* OwningDatabase = nullptr;

	UArticyObject* getObjInternal(const ExpressoType& Id_CloneId) const;

	static void PrintInternal(const FString& msg);
};

template <typename ... ArgTypes>
void UArticyExpressoScripts::print(const FString& Msg, ArgTypes... Args)
{
	auto msg = Msg;
	
	auto arr = TArray<ExpressoType>{ Args... };
	for(int i = 0; i < arr.Num(); ++i)
		msg = msg.Replace(*FString::Printf(TEXT("{%d}"), i), *FString{ arr[i] });

	PrintInternal(msg);
}
