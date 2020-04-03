//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObject.h"

//#include "PredefinedTypes.generated.h"

class UArticyImportData;

/**
 * Use this to define a predefined type which uses the same string for declaring a class and exposing a variable
 * The type must provide a constructor which takes a TSharedPtr<FJsonValue>, used as deserializer.
 */
#define PREDEFINE_TYPE(Type) new ArticyPredefinedTypeInfo<Type>(#Type, #Type, TEXT(""), [](PROP_SETTER_PARAMS) { return Type{ Json }; })
//same as PREDEFINED_TYPE, but with a custom DefaultValue and Deserializer.
#define PREDEFINE_TYPE_EXT(Type, DefaultValue, Deserializer) new ArticyPredefinedTypeInfo<Type>(#Type, #Type, DefaultValue, Deserializer)
//same as PREDEFINED_TYPE_EX, but creates a ArticyObjectTypeInfo, which has a default deserializer
#define PREDEFINE_ARTICYOBJECT_TYPE(Type) new ArticyObjectTypeInfo<Type, Type*>(#Type, #Type"*")

#define PROP_SETTER_PARAMS UArticyBaseObject* Model, const FString& Path, const TSharedPtr<FJsonValue> Json
#define ARRAY_SETTER_PARAMS UArticyBaseObject* Model, const FString& Path, const TArray<TSharedPtr<FJsonValue>> JsonArray
#define PROP_SETTER_ARGS Model, Path, Json

struct ArticyPredefinedTypeBase
{
public:
	ArticyPredefinedTypeBase(FString TypeName, FString PropTypeName, FString DefaultValue) : CppType(TypeName), CppPropertyType(PropTypeName), CppDefaultValue(DefaultValue) { }
	virtual ~ArticyPredefinedTypeBase() = default;

public:
	UPROPERTY(VisibleAnywhere, Category="PredefinedTypeInfo")
	const FString CppType = "";
	UPROPERTY(VisibleAnywhere, Category="PredefinedTypeInfo")
	const FString CppPropertyType = "";
	UPROPERTY(VisibleAnywhere, Category="PredefinedTypeInfo")
	const FString CppDefaultValue = "";

	virtual void SetProp(FName Property, PROP_SETTER_PARAMS) { ensureMsgf(false, TEXT("SetProp not implemented in derived class!")); }
	virtual void SetArray(FName ArrayProperty, ARRAY_SETTER_PARAMS) { ensureMsgf(false, TEXT("SetProp not implemented in derived class!")); }
};

/**
 * This struct contains information about the type (as c++ type and as string) used
 * to declare a class/struct/enum as well as the type to expose an instance of that
 * type.
 * It also defines how to initialize a property on a UArticyObject from json,
 * using UArticyObject->setProp.
 */
template<typename Type, typename PropType = Type>
struct ArticyPredefinedTypeInfo : public ArticyPredefinedTypeBase
{
public:
	ArticyPredefinedTypeInfo(FString TypeName, FString PropTypeName, FString DefaultValue, PropType (*Deserializer)(PROP_SETTER_PARAMS))
		: ArticyPredefinedTypeBase(TypeName, PropTypeName, DefaultValue), Deserializer(Deserializer)
	{ }

	virtual ~ArticyPredefinedTypeInfo() = default;
	
public:

	typedef PropType (*TDeserializer)(PROP_SETTER_PARAMS);
	/** The deserializer used to deserialize JSON to PropType. */
	TDeserializer Deserializer = nullptr;

public:

	/** This is used to set a property of type PropType. */
	void SetProp(FName Property, PROP_SETTER_PARAMS) override
	{
		if(ensure(Deserializer))
			Model->SetProp<PropType>(Property, Deserializer(PROP_SETTER_ARGS));
	}

	/** This is used to set an array property with ItemType = PropType. */
	void SetArray(FName ArrayProperty, ARRAY_SETTER_PARAMS) override
	{
		if(!ensure(Deserializer))
			return;

		static TArray<PropType> propArray;

		propArray.Reset(JsonArray.Num());
		for(auto j : JsonArray)
			propArray.Add(Deserializer(Model, Path, j));

		Model->SetProp(ArrayProperty, propArray);
	}
};

/**
 * This ArticyPredefinedTypeInfo can be used for all UArticyBaseObjects.
 * It defines a default Deserializer.
 */
template<typename Type, typename PropType = Type>
struct ArticyObjectTypeInfo : ArticyPredefinedTypeInfo<Type, PropType>
{
public:
	ArticyObjectTypeInfo(FString TypeName, FString PropTypeName) : ArticyPredefinedTypeInfo<Type, PropType>(TypeName, PropTypeName, TEXT("nullptr"), [](PROP_SETTER_PARAMS)
	{
		auto val = NewObject<Type>(Model);
		val->InitFromJson(Json);
		
		auto prim = Cast<UArticyPrimitive>(val);
		if(prim)
			Model->AddSubobject(prim);

		return val;
	}) { static_assert(std::is_base_of<UArticyBaseObject, Type>::value, "ArticyObjectTypeInfo might only be used for UArticyBaseObject!"); }

	virtual ~ArticyObjectTypeInfo() = default;
};

/**
 * Just a container to hold information about the predefined Articy types.
 */
struct FArticyPredefTypes
{
public:
	FArticyPredefTypes();

	static TMap<FName, ArticyPredefinedTypeBase*>& Get() { return StaticInstance.Types; }
	static ArticyPredefinedTypeInfo<uint8>* GetEnum() { return StaticInstance.Enum.Get(); }
	static bool IsPredefinedType(const FName& OriginalType);

private:

	/** all predefined types */
	TMap<FName, ArticyPredefinedTypeBase*> Types;
	/** generic type for enums, only used for setting enum properties (as uint8) */
	TSharedPtr<ArticyPredefinedTypeInfo<uint8>> Enum;

	static FArticyPredefTypes StaticInstance;
};
