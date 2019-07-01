//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyImporter.h"
#include "ArticyPluginSettings.h"
#include "ArticyPluginSettingsCustomization.h"

#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Developer/Settings/Public/ISettingsContainer.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "ArticyImporterHelpers.h"
#include <Dialogs.h>
#include <SWindow.h>

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

bool FArticyImporterModule::IsImportQueued()
{
	return bIsImportQueued;
}

void FArticyImporterModule::SetImportCreationData(ArticyImporterHelpers::ArticyImportCreationData creationData)
{
	ImportDataCreationData = creationData;
}

void FArticyImporterModule::QueueImport()
{
	bIsImportQueued = true;
	FOnMsgDlgResult OnDialogClosed;
	FText Message = LOCTEXT("ImportWhilePlaying", "To import articy:draft data, the play mode has to be quit. Import will begin after exiting play.");
	FText Title = LOCTEXT("ImportWhilePlaying_Title", "Import not possible");
	TSharedRef<SWindow> window = OpenMsgDlgInt_NonModal(EAppMsgType::Ok, Message, Title, OnDialogClosed);
	FEditorDelegates::EndPIE.AddRaw(this, &FArticyImporterModule::TriggerQueuedImport);
}

void FArticyImporterModule::UnqueueImport()
{
	FEditorDelegates::EndPIE.RemoveAll(this);
	bIsImportQueued = false;
	ImportDataCreationData = ArticyImporterHelpers::ArticyImportCreationData();
}

void FArticyImporterModule::TriggerQueuedImport(bool b)
{
	UArticyJSONFactory* factory = NewObject<UArticyJSONFactory>();
	UPackage * package = CreatePackage(nullptr, *(ImportDataCreationData.PackageName));
	factory->ImportObject(ImportDataCreationData.InClass, package, ImportDataCreationData.InName, ImportDataCreationData.Flags, ImportDataCreationData.Filename, ImportDataCreationData.Parms, ImportDataCreationData.bOutOperationCanceled);
	// important to unqueue in the end to reset the state
	UnqueueImport();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArticyImporterModule, ArticyImporter)