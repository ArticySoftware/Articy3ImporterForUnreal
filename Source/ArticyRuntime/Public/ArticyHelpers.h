//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Engine.h"
#include "AssetRegistryModule.h"
#include <sstream>
#include "Dom/JsonValue.h"
#include "UObject/Package.h"
#include "Dom/JsonObject.h"

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

	static const FString ArticyFolder = FString(TEXT("/Game/ArticyContent/"));
	static const FString ArticyAssetsFolder = ArticyFolder / TEXT("Resources");
	static const FString ArticyGeneratedFolder = ArticyFolder / TEXT("Generated");
	static const FString ArticyGeneratedPackagesFolder = ArticyGeneratedFolder / TEXT("Packages");
	static const FString ArticyFolderRelativeToContent = FString(TEXT("ArticyContent/"));
	static const FString ArticyGeneratedFolderRelativeToContent = ArticyFolderRelativeToContent / TEXT("Generated");
	static const FString ArticyGeneratedPackagesFolderRelativeToContent = ArticyGeneratedFolderRelativeToContent / TEXT("Packages");


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
		obj->TryGetNumberField("x", X);
		obj->TryGetNumberField("y", Y);

		return FVector2D{ static_cast<float>(X), static_cast<float>(Y) };
	}

	inline FLinearColor ParseColorFromJson(const TSharedPtr<FJsonValue> Json)
	{
		if(!Json.IsValid() || !ensure(Json->Type == EJson::Object))
			return FLinearColor{};

		double R, G, B, A = 1.0;

		auto obj = Json->AsObject();
		obj->TryGetNumberField("r", R);
		obj->TryGetNumberField("g", G);
		obj->TryGetNumberField("b", B);
		obj->TryGetNumberField("a", A);

		return FLinearColor{ static_cast<float>(R), static_cast<float>(G), static_cast<float>(B), static_cast<float>(A) };
	}
}


