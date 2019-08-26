//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "CoreMinimal.h"
#include "SharedPointer.h"
#include "Object.h"
#include "Archive.h"
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

	// PackageLoadSettings is a temp fix. This should be on its own panel later on.
	// Problems: Tried custom UI, but due to serialization behavior in project settings not advisable due to:
	// ListView (for custom list UI) requires UObject (not config compatible), TSharedRef or TSharedPtr (both not UPROPERTY compatible)
	// #TODO create custom widgets based on TMap below, hide map itself, modify map with custom widgets OR write custom list view

	/** The packages to load automatically upon cloning the database. DO NOT MODIFY PACKAGE NAMES MANUALLY */
	UPROPERTY(EditAnywhere, EditFixedSize, config, Category=RuntimeSettings, meta=(DisplayName="Packages to load by default"))
	TMap<FString, bool> PackageLoadSettings;

	bool IsLoadingPackageByDefault(FString packageName) const;

	bool doesPackageSettingExist(FString packageName);
	/* --------------------------------------------------------------------- */

	static const UArticyPluginSettings* Get();

	void UpdatePackageLoadSettings();

};
