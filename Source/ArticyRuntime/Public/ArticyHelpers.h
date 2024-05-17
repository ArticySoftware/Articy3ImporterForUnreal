//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include <sstream>

#include "ArticyPluginSettings.h"
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0 
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif
#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "ArticyRuntimeModule.h"

namespace ArticyHelpers
{

#define STRINGIFY(x) #x

#define JSON_SECTION_SETTINGS TEXT("Settings")
#define JSON_SECTION_PROJECT TEXT("Project")
#define JSON_SECTION_GLOBALVARS TEXT("GlobalVariables")
#define JSON_SECTION_SCRIPTMEETHODS TEXT("ScriptMethods")
#define JSON_SECTION_OBJECTDEFS TEXT("ObjectDefinitions")
#define JSON_SECTION_PACKAGES TEXT("Packages")
#define JSON_SECTION_HIERARCHY TEXT("Hierarchy")

/** Creates a new FJsonObject with name "x" from json->GetObjectField("x") */
#define JSON_OBJECT(json, x) TSharedPtr<FJsonObject> x = json->GetObjectField(TEXT(#x))
/** Tries to get the object with name "x" and if it's an object, executes body. */
#define JSON_TRY_OBJECT(json, x, body) static_assert(!std::is_const<decltype(x)>::value, #x " is const!"); \
	{ const TSharedPtr<FJsonObject> *obj; \
	if(json->TryGetObjectField(TEXT(#x), obj)) \
	{ body } }

/** Tries to get the bool with name "x" from json and stores it into 'x'. */
#define JSON_TRY_BOOL(json, x) static_assert(!std::is_const<decltype(x)>::value, #x " is const!"); \
	json->TryGetBoolField(TEXT(#x), x)

/** Tries to get the string with name "x" from json and stores it into 'x'. */
#define JSON_TRY_STRING(json, x) static_assert(!std::is_const<decltype(x)>::value, #x " is const!"); \
	json->TryGetStringField(TEXT(#x), x)
/** Tries to get the string with name "x" from json and stores it into 'x'. */
#define JSON_TRY_FNAME(json, x) { FString str; if(json->TryGetStringField(TEXT(#x), str)) x = *str; }

#define JSON_TRY_TEXT(json, x) { FString str; if(json->TryGetStringField(TEXT(#x), str)) x = FText::FromString(str); }

/** Tries to get the string with name "x" from json, converts it to uint64 and stores it into 'x' of type FArticyId. */
#define JSON_TRY_HEX_ID(json, x) static_assert(std::is_same<decltype(x), FArticyId>::value, #x " is not a uint64!"); \
	{ FString hex; \
	json->TryGetStringField(TEXT(#x), hex); \
	x = ArticyHelpers::HexToUint64(hex); }

/** Tries to get all the elements in an array with name "x" from json, and iterates over them. */
#define JSON_TRY_ARRAY(json, x, loopBody) \
	static_assert(!std::is_const<decltype(x)>::value, #x " is const!"); \
	{ const TArray<TSharedPtr<FJsonValue>>* items; \
	if(json->TryGetArrayField(TEXT(#x), items)) \
	for(const auto item : *items) \
	{ loopBody } }

#define JSON_TRY_STRING_ARRAY(json, x) JSON_TRY_ARRAY(json, x, x.Add(item->AsString()); )

/** Tries to get the int with name "x" from json and stores it into 'x'. */
#define JSON_TRY_INT(json, x) json->TryGetNumberField(TEXT(#x), x)

/** Tries to get the int with name "x" from json and stores it into 'x'. */
#define JSON_TRY_FLOAT(json, x) { double d##x; json->TryGetNumberField(TEXT(#x), d##x); x = d##x; }

/** Tries to get the int with name "x" from json and stores it into 'x'. */
#define JSON_TRY_ENUM(json, x) int val; if(json->TryGetNumberField(TEXT(#x), val)) x = static_cast<decltype(x)>(val);

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

	inline FString GetArticyFolder()
	{
		return GetDefault<UArticyPluginSettings>()->ArticyDirectory.Path;
	}

	inline FString GetArticyResourcesFolder()
	{
		return GetArticyFolder() / TEXT("ArticyContent") / TEXT("Resources");
	}

	inline FString GetArticyGeneratedFolder()
	{
		return GetArticyFolder() / TEXT("ArticyContent") / TEXT("Generated");
	}

	inline uint64 HexToUint64(FString str) { return FCString::Strtoui64(*str, nullptr, 16); }
	inline FString Uint64ToHex(uint64 id)
	{
		std::stringstream stream;
		stream << "0x" << std::hex << id;
		return UTF8_TO_TCHAR(stream.str().c_str());
	}

	inline FString Uint64ToObjectString(uint64 id)
	{
		std::ostringstream stream;
		stream << id << "_0";
		return UTF8_TO_TCHAR(stream.str().c_str());
	}

	inline FVector2D ParseFVector2DFromJson(const TSharedPtr<FJsonValue> Json)
	{
		if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
			return FVector2D{};

		double X = 0, Y = 0;

		auto obj = Json->AsObject();
		obj->TryGetNumberField(TEXT("x"), X);
		obj->TryGetNumberField(TEXT("y"), Y);

		return FVector2D{ static_cast<float>(X), static_cast<float>(Y) };
	}

	inline FMatrix ParseFMatrixFromJson(const TSharedPtr<FJsonValue> Json)
	{
		if (!Json.IsValid() || !ensure(Json->Type == EJson::Array))
			return FMatrix::Identity;

		auto JsonArray = Json->AsArray();
		if (!ensure(JsonArray.Num() == 9))
			return FMatrix::Identity;

		TArray< float > FloatArray = TArray<float>();
		for (auto& JsonFloatValue : JsonArray)
		{
			FloatArray.Add(static_cast<float>(JsonFloatValue->AsNumber()));
		}

		// Take the 2D 3x3 Matrix from Articy:draft and conver it to a 3D 4x4 Matrix for Unreal
		return FMatrix{
			FPlane{FloatArray[0], FloatArray[1], FloatArray[2], 0.f},
			FPlane{FloatArray[3], FloatArray[4], FloatArray[5], 0.f},
			FPlane{0.f, 0.f, FloatArray[8], 0.f},
			// Translation values need to be moved over as they're always on the last column of the matrix
			FPlane{FloatArray[6], FloatArray[7], 0.f, 1.f},
		};
	}

	inline FLinearColor ParseColorFromJson(const TSharedPtr<FJsonValue> Json)
	{
		if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
			return FLinearColor{};

		double R, G, B, A = 1.0;

		auto obj = Json->AsObject();
		obj->TryGetNumberField(TEXT("r"), R);
		obj->TryGetNumberField(TEXT("g"), G);
		obj->TryGetNumberField(TEXT("b"), B);
		obj->TryGetNumberField(TEXT("a"), A);

		return FLinearColor{ static_cast<float>(R), static_cast<float>(G), static_cast<float>(B), static_cast<float>(A) };
	}
}


