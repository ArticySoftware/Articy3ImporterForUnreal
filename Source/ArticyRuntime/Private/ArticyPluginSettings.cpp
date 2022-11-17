//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyPluginSettings.h"
#include "Modules/ModuleManager.h"
#include "ArticyDatabase.h"
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0 
#include "AssetRegistry/AssetRegistryModule.h"
#else
#include "AssetRegistryModule.h"
#endif
#include "Misc/ConfigCacheIni.h"

UArticyPluginSettings::UArticyPluginSettings()
{
	bCreateBlueprintTypeForScriptMethods = true;
	bKeepDatabaseBetweenWorlds = true;
	bKeepGlobalVariablesBetweenWorlds = true;
	bConvertUnityToUnrealRichText = false;
	bVerifyArticyReferenceBeforeImport = true;
	bUseLegacyImporter = false;
	
	bSortChildrenAtGeneration = false;
	ArticyDirectory.Path = TEXT("/Game");
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
	TWeakObjectPtr<UArticyDatabase> ArticyDatabase = UArticyDatabase::GetMutableOriginal();

	if (!ArticyDatabase.IsValid())
	{
		return;
	}
	
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
	TWeakObjectPtr<UArticyDatabase> OriginalDatabase = UArticyDatabase::GetMutableOriginal();

	for(FString PackageName : OriginalDatabase->GetImportedPackageNames())
	{
		OriginalDatabase->ChangePackageDefault(FName(*PackageName), GetDefault<UArticyPluginSettings>()->PackageLoadSettings[PackageName]);
	}
}
#if WITH_EDITOR
void UArticyPluginSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	GConfig->Flush(false, GEngineIni);
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

// @TODO 
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION < 25
void UArticyPluginSettings::PostReloadConfig(UProperty* PropertyThatWasLoaded)
#else
void UArticyPluginSettings::PostReloadConfig(FProperty* PropertyThatWasLoaded)
#endif
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
	GConfig->Flush(false, GEngineIni);
}

void UArticyPluginSettings::PostInitProperties()
{
	Super::PostInitProperties();
	GConfig->Flush(false, GEngineIni);
}

void UArticyPluginSettings::PostTransacted(const FTransactionObjectEvent& TransactionEvent)
{
	Super::PostTransacted(TransactionEvent);
	GConfig->Flush(false, GEngineIni);
}
#endif