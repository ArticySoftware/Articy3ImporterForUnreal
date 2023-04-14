#pragma once
#include "ScriptMethod.generated.h"

struct FAIDScriptMethodParameter;

USTRUCT()
struct FAIDScriptMethod
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString Name;
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString BlueprintName;
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	bool bIsOverloadedFunction = false;

	/** A list of parameters (type + parameter name), to be used in a method declaration. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FAIDScriptMethodParameter> ParameterList;
	/** A list of arguments (values), including a leading comma, to be used when calling a method. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FString> ArgumentList;
	/** A list of parameters (original types), used for generating the blueprint function display name. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FString> OriginalParameterTypes;

	const FString& GetCPPReturnType() const;
	const FString& GetCPPDefaultReturn() const;
	const FString GetCPPParameters() const;
	const FString GetArguments() const;
	const FString GetOriginalParametersForDisplayName() const;

	void ImportFromJson(TSharedPtr<FJsonObject> Json, TSet<FString>& OverloadedMethods);

private:
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString ReturnType;
};