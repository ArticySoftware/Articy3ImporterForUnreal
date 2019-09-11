//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "ArticyPluginSettings.h"
#include "ModuleManager.h"
#include "AssetRegistryModule.h"
#include "ArticyDatabase.h"

UArticyPluginSettings::UArticyPluginSettings()
{
	bCreateBlueprintTypeForScriptMethods = true;
	bKeepDatabaseBetweenWorlds = true;
	bKeepGlobalVariablesBetweenWorlds = true;

	// update package load settings after all files have been loaded
	FAssetRegistryModule& assetRegistry = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	assetRegistry.Get().OnFilesLoaded().AddUObject(this, &UArticyPluginSettings::UpdatePackageSettings);
}

bool UArticyPluginSettings::DoesPackageSettingExist(FString packageName)
{
	return PackageLoadSettings.Contains(packageName);
}

const UArticyPluginSettings* UArticyPluginSettings::Get()
{
	static TWeakObjectPtr<UArticyPluginSettings> Settings;

	if (!Settings.IsValid())
	{
		Settings = TWeakObjectPtr<UArticyPluginSettings>(NewObject<UArticyPluginSettings>());
	}

	return Settings.Get();
}

void UArticyPluginSettings::UpdatePackageSettings()
{
	UArticyDatabase* articyDatabase = UArticyDatabase::GetMutableOriginal();

	TArray<FString> importedPackageNames = articyDatabase->GetImportedPackageNames();

	// remove outdated settings
	TArray<FString> currentNames;
	PackageLoadSettings.GenerateKeyArray(currentNames);

	for (FString name : currentNames)
	{
		// if the old name isn't contained in the new packages, remove its loading rule
		if(!importedPackageNames.Contains(name))
		{
			PackageLoadSettings.Remove(name);
		}
	}

	for (FString name : importedPackageNames)
	{
		// if the new name isn't contained in the serialized data, add it with its default package value
		if (!currentNames.Contains(name))
		{
			PackageLoadSettings.Add(name, articyDatabase->IsPackageDefaultPackage(name));
		}
	}

	// apply previously existing settings for the packages so that user tweaked values don't get reset
	ApplyPreviousSettings();
}

void UArticyPluginSettings::ApplyPreviousSettings()
{
	// restore the package default settings with the cached data of the plugin settings
	UArticyDatabase* OriginalDatabase = UArticyDatabase::GetMutableOriginal();

	for(FString packageName : OriginalDatabase->GetImportedPackageNames())
	{
		OriginalDatabase->ChangePackageDefault(FName(*packageName), GetDefault<UArticyPluginSettings>()->PackageLoadSettings[packageName]);
	}
}
