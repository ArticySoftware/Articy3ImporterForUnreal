//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.

#pragma once

#include "CoreMinimal.h"

/*
 * Handle plugin generation while requested by code generator at import stage.
 */
class PluginGenerator
{
protected:
	inline static const FString PluginName = FString(TEXT("ArticyGenerated"));
	
public:
	static FString GetPluginName() { return PluginName; }
	static bool IsPluginLoaded();
	static void GeneratePlugin();
};
