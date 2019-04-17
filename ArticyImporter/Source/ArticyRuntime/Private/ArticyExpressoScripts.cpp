//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyExpressoScripts.h"
#include "ArticyRuntime.h"
#include "ArticyFlowPlayer.h"

TMap<FName, ExpressoType::Definition> ExpressoType::Definitions;

ExpressoType::ExpressoType(UArticyBaseObject* Object, const FName &Property)
{
	auto propName = Property.ToString();
	Object = TryFeatureReroute(Object, propName);

	if(!Object)
		return;

	auto prop = Object->GetProperty(*propName);
	if(!ensure(prop))
		return;

	FString itemType;
	FName type = *prop->GetCPPType(&itemType);

	auto factory = GetDefinition(type).Factory;
	if(ensureMsgf(factory, TEXT("Property %s has unknown type %s!"), *propName, *type.ToString()))
		*this = factory(Object, prop);
}

//---------------------------------------------------------------------------//

ExpressoType::ExpressoType(const bool& Value)
{
	BoolValue = Value;
	Type = Bool;
}

ExpressoType::ExpressoType(const int64& Value)
{
	IntValue = Value;
	Type = Int;
}

ExpressoType::ExpressoType(const double& Value)
{
	FloatValue = Value;
	Type = Float;
}

ExpressoType::ExpressoType(const FString& Value)
{
	StringValue = Value;
	Type = String;
}

//========================================//

ExpressoType::ExpressoType(const UArticyPrimitive* Object)
{
	Type = String;

	if(!Object)
		return;

	StringValue = FString::Printf(TEXT("%d_%d"), Object->GetId(), Object->GetCloneId());
}

ExpressoType::ExpressoType(const UArticyString& Value)
{
	Type = String;
	StringValue = Value.Get();
}

ExpressoType::ExpressoType(const UArticyInt & Value)
{
	Type = Int;
	IntValue = Value.Get();
}

ExpressoType::ExpressoType(const UArticyBool & Value)
{
	Type = Bool;
	BoolValue = Value.Get();
}

ExpressoType::operator bool() const
{
	ensure(Type == Bool);
	return GetBool();
}

ExpressoType::operator int64() const
{
	ensure(Type == Float || Type == Int);

	if (Type == Float)
		return GetFloat();

	return GetInt();
}

ExpressoType::operator double() const
{
	ensure(Type == Float || Type == Int);

	if(Type == Float)
		return GetFloat();

	return GetInt();
}

ExpressoType::operator FString() const
{
	ensure(Type == String);
	return GetString();
}

//---------------------------------------------------------------------------//
//===========================================================================//

ExpressoType ExpressoType::operator-() const
{
	switch (Type)
	{
	case Undefined:
		break;
	case Bool:
		return ExpressoType(!GetBool());
	case Int:
		return ExpressoType(-GetInt());
	case Float:
		return ExpressoType(-GetFloat());
	case String:
		return ExpressoType(FString("")); // update for VS2019: Fixed Error C4800. Was just "" before

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}
	
	return ExpressoType{};
}

//---------------------------------------------------------------------------//

bool ExpressoType::operator==(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Undefined:
		break;

	case Bool:
		return GetBool() == Other.GetBool();
	case Int:
		return GetInt() == Other.GetInt();
	case Float:
		return GetFloat() == Other.GetFloat();
	case String:
		return GetString() == Other.GetString();

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return false;
}

bool ExpressoType::operator!=(const ExpressoType& Other) const
{
	return !(*this == Other);
}

bool ExpressoType::operator<(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Undefined:
		break;

	case Bool:
		return GetBool() < Other.GetBool();
	case Int:
		return GetInt() < Other.GetInt();
	case Float:
		return GetFloat() < Other.GetFloat();
	case String:
		return GetString() < Other.GetString();

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return false;
}

bool ExpressoType::operator>(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Bool:
		return GetBool() > Other.GetBool();
	case Int:
		return GetInt() > Other.GetInt();
	case Float:
		return GetFloat() > Other.GetFloat();
	case String:
		return GetString() > Other.GetString();

	case Undefined:
		break;

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return false;
}

//---------------------------------------------------------------------------//

ExpressoType ExpressoType::operator&&(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Bool:
		return ExpressoType(GetBool() && Other.GetBool());
	case Int:
		return ExpressoType(GetInt() && Other.GetInt());
	case Float:
		return ExpressoType(GetFloat() && Other.GetFloat());

	case String:
	case Undefined:
		break;

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return ExpressoType{};
}

ExpressoType ExpressoType::operator||(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Bool:
		return ExpressoType(GetBool() || Other.GetBool());
	case Int:
		return ExpressoType(GetInt() || Other.GetInt());
	case Float:
		return ExpressoType(GetFloat() || Other.GetFloat());

	case String:
	case Undefined:
		break;

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return ExpressoType{};
}

ExpressoType ExpressoType::operator^(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Bool:
		return ExpressoType(GetBool() ^ Other.GetBool());
	case Int:
		return ExpressoType(GetInt() ^ Other.GetInt());

	case Float:
	case String:
	case Undefined:
		break;

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return ExpressoType{};
}

ExpressoType ExpressoType::operator+(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Undefined:
		break;

	case Bool:
		return ExpressoType(GetBool() || Other.GetBool());
	case Int:
		return ExpressoType(GetInt() + Other.GetInt());
	case Float:
		return ExpressoType(GetFloat() + Other.GetFloat());
	case String:
		return ExpressoType(GetString() + Other.GetString());

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return ExpressoType{};
}

ExpressoType ExpressoType::operator-(const ExpressoType& Other) const
{
	return *this + (-Other);
}

ExpressoType ExpressoType::operator*(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Bool:
		return *this && Other;
	case Int:
		return ExpressoType(GetInt() * Other.GetInt());
	case Float:
		return ExpressoType(GetFloat() * Other.GetFloat());

	case String:
	case Undefined:
		break;

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return ExpressoType{};
}

ExpressoType ExpressoType::operator/(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Int:
		return ExpressoType(GetInt() / Other.GetInt());
	case Float:
		return ExpressoType(GetFloat() / Other.GetFloat());

	case Bool:
	case String:
	case Undefined:
		break;

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return ExpressoType{};
}

ExpressoType ExpressoType::operator%(const ExpressoType& Other) const
{
	switch (Type)
	{
	case Int:
		return ExpressoType(GetInt() % Other.GetInt());
	case Float:
		float OutIntPart;
		return ExpressoType(FMath::Modf(GetFloat(), &OutIntPart));

	case Bool:
	case String:
	case Undefined:
		break;

	default:
		ensureMsgf(false, TEXT("Unknown ArticyExpressoType!"));
	}

	return ExpressoType{};
}

/*
ExpressoType& ExpressoType::operator++()
{
	*this = this + 1;
	return *this;
}

ExpressoType ExpressoType::operator++(int)
{
	auto tmp = *this;
	*this = this + 1;
	return tmp;
}

ExpressoType& ExpressoType::operator--()
{
	*this = this - 1;
	return *this;
}

ExpressoType ExpressoType::operator--(int)
{
	auto tmp = *this;
	*this = this - 1;
	return tmp;
}
*/

//---------------------------------------------------------------------------//

const ExpressoType::Definition& ExpressoType::GetDefinition(const FName& CppType) const
{
	if(Definitions.Num() == 0)
	{
		#define ADD_DEFINITION(Type) AddDefinition<Type>(#Type);

		ADD_DEFINITION(bool);
		
		ADD_DEFINITION(int8);
		ADD_DEFINITION(int16);
		ADD_DEFINITION(int);
		ADD_DEFINITION(int32);
		ADD_DEFINITION(int64);

		//although they have the same size as the signed counterparts, these
		//are needed because we need to match them (i.e. need to be in the map)
		ADD_DEFINITION(uint8);
		ADD_DEFINITION(uint16);
		ADD_DEFINITION(uint32);
		ADD_DEFINITION(uint64);
			
		ADD_DEFINITION(float);
		ADD_DEFINITION(double);

		ADD_DEFINITION(FString);
		ADD_DEFINITION(FText);
		ADD_DEFINITION(FName);

		ADD_DEFINITION(FArticyId);
	}

	auto def = Definitions.Find(CppType);
	if(def)
		return *def;

	static const auto Empty = Definition{};
	return Empty;
}

void ExpressoType::SetValue(UArticyBaseObject* Object, FString Property) const
{
	Object = TryFeatureReroute(Object, Property);

	if(!Object)
		return;

	auto prop = Object->GetProperty(*Property);
	if(!ensure(prop))
	{
		UE_LOG(LogArticyRuntime, Warning, TEXT("Property %s not found on Object %s!"), *Property, *Object->GetName());
		return;
	}

	FString itemType;
	FName type = *prop->GetCPPType(&itemType);

	auto setter = GetDefinition(type).Setter;

	if(ensureMsgf(setter, TEXT("Property %s has unknown type %s!"), *Property, *type.ToString()))
		setter(Object, prop, *this);
}

UArticyBaseObject* ExpressoType::TryFeatureReroute(UArticyBaseObject* Object, FString& Property)
{
	if(Object)
	{
		FString feature;
		if(Property.Split(TEXT("."), &feature, &Property))
		{
			//the property contains a dot
			//take the part before the dot to extract the feature, and use it as Object to get the actual property from
			Object = Object->GetProp<UArticyBaseFeature*>(*feature);
			if(!ensure(Object))
			{
				UE_LOG(LogArticyRuntime, Warning, TEXT("Feature %s on Object %s is null, cannot access property %s!"), *feature, *Object->GetName(), *Property);
				return nullptr;
			}

		}
	}

	return Object;
}

//---------------------------------------------------------------------------//

bool UArticyExpressoScripts::Evaluate(const int& ConditionFragmentHash, UArticyGlobalVariables* GV, UObject* MethodProvider) const
{
	SetGV(GV);
	UserMethodsProvider = MethodProvider;

	auto condition = Conditions.Find(ConditionFragmentHash);
	return ensure(condition) && (*condition)();
}

bool UArticyExpressoScripts::Execute(const int& InstructionFragmentHash, UArticyGlobalVariables* GV, UObject* MethodProvider) const
{
	SetGV(GV);
	UserMethodsProvider = MethodProvider;

	auto instruction = Instructions.Find(InstructionFragmentHash);
	if(ensure(instruction))
	{
		(*instruction)();
		return true;
	}

	return false;
}

UArticyObject* UArticyExpressoScripts::getObj(const FString& NameOrId, const uint32& CloneId) const
{
	if(NameOrId.StartsWith(TEXT("0x")))
		return OwningDatabase->GetObject<UArticyObject>(FArticyId{ ArticyHelpers::HexToUint64(NameOrId) }, CloneId);
	if(NameOrId.IsNumeric())
		return OwningDatabase->GetObject<UArticyObject>(FArticyId{ FCString::Strtoui64(*NameOrId, NULL, 10) }, CloneId);

	return OwningDatabase->GetObjectByName(*NameOrId, CloneId);
}

UArticyObject* UArticyExpressoScripts::getObjInternal(const ExpressoType& Id_CloneId) const
{
	//only works for strings!
	if(!ensureMsgf(Id_CloneId.Type == ExpressoType::String, TEXT("getObj(Id_CloneId) only works for string-ExpressoType!")))
		return nullptr;

	//parse id and cloneId from the compound id
	FString Id, CloneId;
	if(!Id_CloneId.GetString().Split("_", &Id, &CloneId))
		return nullptr;

	//finally get the object by id and cloneId
	return getObj(Id, FCString::Atoi(*CloneId));
}

void UArticyExpressoScripts::PrintInternal(const FString& msg)
{
	UE_LOG(LogArticyRuntime, Log, TEXT("%s"), *msg);
}

void UArticyExpressoScripts::setProp(UArticyBaseObject* Object, const FString& Property, const ExpressoType& Value)
{
	Value.SetValue(Object, Property);
}

void UArticyExpressoScripts::setProp(const ExpressoType& Id_CloneId, const FString& Property, const ExpressoType& Value) const
{
	setProp(getObjInternal(Id_CloneId), Property, Value);
}

ExpressoType UArticyExpressoScripts::getProp(UArticyBaseObject* Object, const FName& Property)
{
	return ExpressoType{Object, Property};
}

ExpressoType UArticyExpressoScripts::getProp(const ExpressoType& Id_CloneId, const FName& Property) const
{
	return getProp(getObjInternal(Id_CloneId), Property);
}

int UArticyExpressoScripts::random(int Min, int Max)
{
	return FMath::RandRange(Min, Max);
}

int UArticyExpressoScripts::random(int Max)
{
	return FMath::RandRange(0, Max);
}

float UArticyExpressoScripts::random(float Min, float Max)
{
	return FMath::FRandRange(Min, Max);
}

float UArticyExpressoScripts::random(float Max)
{
	return FMath::FRandRange(0, Max);
}
