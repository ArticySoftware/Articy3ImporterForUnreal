#pragma once
#include "MethodParameter.generated.h"

USTRUCT()
struct FAIDScriptMethodParameter
{
	GENERATED_BODY()

	FAIDScriptMethodParameter()
	{
	}

	FAIDScriptMethodParameter(FString InType, FString InName) : Type(InType), Name(InName)
	{
	}

	UPROPERTY(VisibleAnywhere, Category = "ScriptMethods")
	FString Type;

	UPROPERTY(VisibleAnywhere, Category = "ScriptMethods")
	FString Name;
};