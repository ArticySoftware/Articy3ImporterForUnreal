//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "UObject/Object.h"
#include "Engine/EngineTypes.h"
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

	/** If true will attempt to sort the children using the exported position properties. Will be slower */
	UPROPERTY(EditAnywhere, config, Category = ImportSettings, meta = (DisplayName = "Sort children when importing"))
	bool bSortChildrenAtGeneration;

	/** The directory where ArticyContent will be generated and assets are looked for (when using ArticyAsset)
	 *	Also used to search for the .articyue4 file to regenerate the import asset.
	 *. Automatically set to the location of the import asset during import.
	 *  Change only to manually apply a fix!
	 */
	UPROPERTY(VisibleAnywhere, config, Category = ImportSettings, meta = (DisplayName = "Articy Directory", ContentDir, LongPackageName))
	FDirectoryPath ArticyDirectory;
	
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


#if WITH_EDITOR
	/** Those functions will primarily flush the config so changes get written to the config file */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#if ENGINE_MINOR_VERSION < 25
	virtual void PostReloadConfig(class UProperty* PropertyThatWasLoaded) override;
#else
	virtual void PostReloadConfig(class FProperty* PropertyThatWasLoaded) override;
#endif
	virtual void PostInitProperties() override;

	virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;
#endif

};
