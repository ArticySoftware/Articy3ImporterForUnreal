//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ADGV.generated.h"

// ------------------ GV Articy types
UENUM()
enum class EArticyType : uint8
{
	ADT_Boolean,
	ADT_Integer,
	ADT_String
};

// ------------------ GV variable struct
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
};

// -------------------- GV Namespace
USTRUCT()
struct FArticyGVNamespace
{
	GENERATED_BODY()
	
	/** The name of this namespace */
	UPROPERTY(VisibleAnywhere, Category="Namespace")
	FString Namespace;
	UPROPERTY(VisibleAnywhere, Category="Namespace")
	FString Description;
	UPROPERTY(VisibleAnywhere, Category="Namespace")
	FString CppTypename;
	UPROPERTY(VisibleAnywhere, Category="Namespace")
	TArray<FArticyGVar> Variables;	
};

// ------------------------ GV root Definition
USTRUCT()
struct ARTICYEDITOR_API FADGV
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="GlobalVariables")
	TArray<FArticyGVNamespace> Namespaces;
};
