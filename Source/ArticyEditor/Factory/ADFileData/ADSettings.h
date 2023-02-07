//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ADSettings.generated.h"

/**
 * 
 */
USTRUCT()
struct FADSettings
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString set_TextFormatter = "";

	UPROPERTY(VisibleAnywhere, Category="Settings")
	bool set_UseScriptSupport = false;

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ExportVersion = "";

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ObjectDefinitionsHash = "";

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString ScriptFragmentsHash = "";
};