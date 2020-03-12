//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "UObject/Object.h"
#include "ArticyPluginSettings.generated.h"

UCLASS(config = Engine, defaultconfig)
class ARTICYRUNTIME_API UArticyPluginSettings : public UObject
{
	GENERATED_BODY() 

public:
	UArticyPluginSettings();

	/**
	 * Exposes the generated method provider interface to Blueprint. 
	 * INFO: Generated functions will be named according to the format <MethodName>_<ArgumentTypes> as a workaround to support overloaded functions in blueprint.
	*/
	UPROPERTY(EditAnywhere, config, Category=ImportSettings, meta=(DisplayName="Create Blueprint type for script method interface"))
	bool bCreateBlueprintTypeForScriptMethods;

	/* --------------------------------------------------------------------- */

	/** Keeps one instance of the database for the whole game alive, even if the world changes */
	UPROPERTY(EditAnywhere, config, Category=RuntimeSettings, meta=(DisplayName="Keep database between worlds"))
	bool bKeepDatabaseBetweenWorlds;

	/** Keeps one instance of the global variables for the whole game alive, even if the world changes */
	UPROPERTY(EditAnywhere, config, Category=RuntimeSettings, meta=(DisplayName="Keep global variables between worlds"))
	bool bKeepGlobalVariablesBetweenWorlds;

	// internal cached data for data consistency between imports (setting restoration etc.)
	UPROPERTY()
	TMap<FString, bool> PackageLoadSettings;

	bool DoesPackageSettingExist(FString packageName);
	/* --------------------------------------------------------------------- */

	static const UArticyPluginSettings* Get();

	void UpdatePackageSettings();

	void ApplyPreviousSettings() const;

};
