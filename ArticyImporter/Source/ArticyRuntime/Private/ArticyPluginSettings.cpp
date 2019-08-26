//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "ArticyPluginSettings.h"

UArticyPluginSettings::UArticyPluginSettings()
{
	bCreateBlueprintTypeForScriptMethods = true;
	bKeepDatabaseBetweenWorlds = true;
	bKeepGlobalVariablesBetweenWorlds = true;

	// update package load settings after all files have been loaded
	FAssetRegistryModule& assetRegistry = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	assetRegistry.Get().OnFilesLoaded().AddUObject(this, &UArticyPluginSettings::UpdatePackageLoadSettings);
}

bool UArticyPluginSettings::IsLoadingPackageByDefault(FString packageName) const
{
	FAssetRegistryModule& assetRegistry = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> importData;
	assetRegistry.Get().GetAssetsByClass(UArticyDatabase::StaticClass()->GetFName(), importData, true);

	check(importData.Num() >= 1);

	UArticyDatabase* articyDatabase = Cast<UArticyDatabase>(importData[0].GetAsset());

	// find the setting and return the user defined value
	if(PackageLoadSettings.Contains(packageName))
	{
		return PackageLoadSettings[packageName];
	}

	// if the setting object for the package doesn't exist, use the default values instead
	return articyDatabase->IsPackageDefaultPackage(packageName);
}

bool UArticyPluginSettings::doesPackageSettingExist(FString packageName)
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

void UArticyPluginSettings::UpdatePackageLoadSettings()
{
	FAssetRegistryModule& assetRegistry = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> importData;
	assetRegistry.Get().GetAssetsByClass(UArticyDatabase::StaticClass()->GetFName(), importData, true);

	if (importData.Num() == 0) return;

	UArticyDatabase* articyDatabase = Cast<UArticyDatabase>(importData[0].GetAsset());

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
		// if the old name isn't contained in the new packages, remove its loading rule
		if (!currentNames.Contains(name))
		{
			PackageLoadSettings.Add(name, articyDatabase->IsPackageDefaultPackage(name));
		}
	}
}
