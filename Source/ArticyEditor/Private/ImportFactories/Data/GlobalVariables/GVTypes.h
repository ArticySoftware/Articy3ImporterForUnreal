#pragma once
#include "GVTypes.generated.h"

UENUM()
enum class EArticyType : uint8
{
	ADT_Boolean,
	ADT_Integer,
	ADT_String
};

USTRUCT()
struct FArticyGVar
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="Variable")
	FString Variable;
	UPROPERTY(VisibleAnywhere, Category="Variable")
	EArticyType Type = EArticyType::ADT_String;
	UPROPERTY(VisibleAnywhere, Category="Variable")
	FString Description;

	UPROPERTY(VisibleAnywhere, Category = "Variable")
	bool BoolValue = false;
	UPROPERTY(VisibleAnywhere, Category = "Variable")
	int IntValue = 0;
	UPROPERTY(VisibleAnywhere, Category = "Variable")
	FString StringValue;

	/** Returns the UArticyVariable type to be used for this variable. */
	FString GetCPPTypeString() const;
	FString GetCPPValueString() const;

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonVar);
};

