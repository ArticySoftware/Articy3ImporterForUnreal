//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyHelpers.h"
#include "ArticyBaseTypes.generated.h"

class UArticyObject;

#pragma region Numeric Types

/**
 * 64-bit ID used for referencing articy objects, exposable to blueprints.
 */
USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyId
{
	GENERATED_BODY()

public:

	FArticyId() = default;

	// ReSharper disable once CppNonExplicitConvertingConstructor
	/** Implicit conversion from uint64. */
	FArticyId(const uint64 &Value) { *this = Value; }
	// ReSharper disable once CppNonExplicitConvertingConstructor
	/** Implicit conversion from FString. */
	FArticyId(const FString &Value) { *this = Value; }

	/** Json deserializer. */
	FArticyId(const TSharedPtr<FJsonValue> Json) { *this = (Json->Type == EJson::String ? ArticyHelpers::HexToUint64(Json->AsString()) : -1); }
	
	//========================================//

	/** The lower 32 bit of the id. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Low = 0;

	/** The higher 32 bit of the id. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 High = 0;

	//========================================//

	uint64 Get() const { return static_cast<uint64>(High) << 32 | static_cast<uint64>(Low); }

	/** Implicit assignment of uint64. */
	FArticyId& operator=(const uint64 &Value)
	{
		//the lower 32 bit are stored in Low, the higher 32 bit in high
		Low = static_cast<int32>(Value & 0xFFFFFFFF);
		High = static_cast<int32>(Value >> 32);

		return *this;
	}
	/** Implicit assignment of FString. */
	FArticyId& operator=(const FString &Value)
	{
		return *this = ArticyHelpers::HexToUint64(Value);
	}

	/** Implicit conversion to uint64. */
	operator uint64 const() const { return Get(); }

	/** Get the referenced object (single runtime copy of the referenced asset). */
	class UArticyPrimitive* GetObject(const UObject* WorldContext) const;
	/** Get the referenced object, cast to T. */
	template<typename T>
	T* GetObject(const UObject* WorldContext) const { return Cast<T>(GetObject(WorldContext)); }

	//========================================//

	/*bool operator==(const FArticyId& Other) const
	{
		return Other.Low == Low && Other.High == High;
	}*/
	friend uint32 GetTypeHash(const FArticyId& Id)
	{
		return Id.Low ^ Id.High;
	}
};

USTRUCT(BlueprintType)
struct FArticyPackage
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Package")
	FString Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Package")
	FString Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Package")
	bool bIsDefaultPackage = false;

	/** All the objects that belong to this model. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UArticyPrimitive>> Objects;
};

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

/**
 * A size given a s width and height.
 */
USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticySize
{
	GENERATED_BODY()
	
public:

	FArticySize() = default;
	FArticySize(TSharedPtr<FJsonValue> Json);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float w;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float h;

	FVector2D AsVector() const { return FVector2D{ w, h }; }
};

/**
 * A rectangle with x/y position and w/h dimensions.
 */
USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyRect
{
	GENERATED_BODY()
	
public:

	FArticyRect() = default;
	FArticyRect(TSharedPtr<FJsonValue> Json);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float y;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float w;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float h;
};

#pragma endregion

//---------------------------------------------------------------------------//

#pragma region enums

UENUM(BlueprintType)
enum class EArticyPreviewImageViewBoxModes : uint8
{
	FromAsset = 0,
	Custom = 1,
};

UENUM()
enum class EArticyLocationAnchorSize : uint8
{
	Small = 0,
	Medium = 1,
	Large = 2,
};

#pragma endregion
