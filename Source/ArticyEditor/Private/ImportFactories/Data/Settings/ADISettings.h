//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ADISettings.generated.h"

/**
 * 
 */
USTRUCT()
struct FADISettings
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