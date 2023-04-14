#pragma once
#include "UserMethods.generated.h"

struct FAIDScriptMethod;

USTRUCT()
struct FAIDUserMethods
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FAIDScriptMethod> ScriptMethods;

	void ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json);
};