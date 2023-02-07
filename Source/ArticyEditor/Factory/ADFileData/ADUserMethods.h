//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ADUserMethods.generated.h"

USTRUCT()
struct FAIDScriptMethodParameter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "ScriptMethods")
	FString Type;

	UPROPERTY(VisibleAnywhere, Category = "ScriptMethods")
	FString Name;
};

USTRUCT()
struct FAIDScriptMethod
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString Name;
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString BlueprintName;
	
	// UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	// bool bIsOverloadedFunction = false;

	/** A list of parameters (type + parameter name), to be used in a method declaration. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FAIDScriptMethodParameter> ParameterList;
	/** A list of arguments (values), including a leading comma, to be used when calling a method. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FString> ArgumentList;
	/** A list of parameters (original types), used for generating the blueprint function display name. */
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FString> OriginalParameterTypes;
	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	FString ReturnType;
};

USTRUCT()
struct ARTICYEDITOR_API FADUserMethods
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="ScriptMethods")
	TArray<FAIDScriptMethod> ScriptMethods;
};
