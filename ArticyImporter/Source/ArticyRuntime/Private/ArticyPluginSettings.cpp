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
	FAssetRegistryModule& AssetRegistry = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistry.Get().OnFilesLoaded().AddUObject(this, &UArticyPluginSettings::UpdatePackageSettings);
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
	UArticyDatabase* ArticyDatabase = UArticyDatabase::GetMutableOriginal();

	if (!ArticyDatabase) return;
	TArray<FString> ImportedPackageNames = ArticyDatabase->GetImportedPackageNames();

	// remove outdated settings
	TArray<FString> CurrentNames;
	PackageLoadSettings.GenerateKeyArray(CurrentNames);

	for (FString Name : CurrentNames)
	{
		// if the old name isn't contained in the new packages, remove its loading rule
		if(!ImportedPackageNames.Contains(Name))
		{
			PackageLoadSettings.Remove(Name);
		}
	}

	for (FString Name : ImportedPackageNames)
	{
		// if the new name isn't contained in the serialized data, add it with its default package value
		if (!CurrentNames.Contains(Name))
		{
			PackageLoadSettings.Add(Name, ArticyDatabase->IsPackageDefaultPackage(Name));
		}
	}

	// apply previously existing settings for the packages so that user tweaked values don't get reset
	ApplyPreviousSettings();
}

void UArticyPluginSettings::ApplyPreviousSettings() const
{
	// restore the package default settings with the cached data of the plugin settings
	UArticyDatabase* OriginalDatabase = UArticyDatabase::GetMutableOriginal();

	for(FString PackageName : OriginalDatabase->GetImportedPackageNames())
	{
		OriginalDatabase->ChangePackageDefault(FName(*PackageName), GetDefault<UArticyPluginSettings>()->PackageLoadSettings[PackageName]);
	}
}
