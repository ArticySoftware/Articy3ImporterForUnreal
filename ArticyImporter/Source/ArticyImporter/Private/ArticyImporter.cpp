//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyImporterPrivatePCH.h"

#include "ArticyPluginSettings.h"
#include "ArticyPluginSettingsCustomization.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"

DEFINE_LOG_CATEGORY(LogArticyImporter)

#define LOCTEXT_NAMESPACE "FArticyImporterModule"

void FArticyImporterModule::StartupModule()
{
	RegisterPluginSettings();
}

void FArticyImporterModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterPluginSettings();
	}
}

void FArticyImporterModule::RegisterPluginSettings()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSectionPtr = SettingsModule->RegisterSettings("Project", "Plugins", "ArticyImporter",
			LOCTEXT("Name", "Articy Importer"),
			LOCTEXT("Description", "Articy Importer Configuration."),
			GetMutableDefault<UArticyPluginSettings>()
		);
	}

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("ArticyPluginSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FArticyPluginSettingsCustomization::MakeInstance));
}

void FArticyImporterModule::UnregisterPluginSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "ArticyImporter");
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArticyImporterModule, ArticyImporter)