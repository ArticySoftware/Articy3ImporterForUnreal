//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyImporter.h"
#include "ArticyPluginSettings.h"
#include "ArticyPluginSettingsCustomization.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Dialogs/Dialogs.h"
#include <Widgets/SWindow.h>
#include "ArticyImporterFunctionLibrary.h"
#include "Editor.h"
#include "Slate/ArticyRefCustomization.h"
#include "ArticyImporterStyle.h"
#include "CodeGeneration/CodeGenerator.h"
#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"

DEFINE_LOG_CATEGORY(LogArticyImporter)

#define LOCTEXT_NAMESPACE "FArticyImporterModule"

void FArticyImporterModule::StartupModule()
{
	RegisterPluginSettings();
	RegisterConsoleCommands();
	RegisterDirectoryWatcher();
	
	// register custom details for ArticyRef struct
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("ArticyRef", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FArticyRefCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	FArticyImporterStyle::Initialize();
}

void FArticyImporterModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterPluginSettings();

		if(ConsoleCommands != nullptr)
		{
			delete ConsoleCommands;
			ConsoleCommands = nullptr;
		}
	}
}

void FArticyImporterModule::RegisterDirectoryWatcher()
{
	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>("DirectoryWatcher");
	DirectoryWatcherModule.Get()->RegisterDirectoryChangedCallback_Handle(CodeGenerator::GetSourceFolder(), IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FArticyImporterModule::OnGeneratedCodeChanged), GeneratedCodeWatcherHandle);
}

void FArticyImporterModule::SetCompleteReimportRequired()
{
	bIsCompleteReimportRequired = true;
}

void FArticyImporterModule::RegisterConsoleCommands()
{
	ConsoleCommands = new FArticyImporterConsoleCommands(*this);
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

void FArticyImporterModule::QueueImport()
{
	bIsImportQueued = true;
	FOnMsgDlgResult OnDialogClosed;
	FText Message = LOCTEXT("ImportWhilePlaying", "To import articy:draft data, the play mode has to be quit. Import will begin after exiting play.");
	FText Title = LOCTEXT("ImportWhilePlaying_Title", "Import not possible");
	TSharedRef<SWindow> Window = OpenMsgDlgInt_NonModal(EAppMsgType::Ok, Message, Title, OnDialogClosed);
	Window->BringToFront(true);
	QueuedImportHandle = FEditorDelegates::EndPIE.AddRaw(this, &FArticyImporterModule::TriggerQueuedImport);
}

ECompleteImportRequiredReason FArticyImporterModule::CheckIsCompleteReimportRequired() const
{
	UArticyImportData* ImportData = nullptr;
	FArticyImporterFunctionLibrary::EnsureImportFile(&ImportData);

	if (!ImportData)
	{
		return ECompleteImportRequiredReason::ImportDataAssetMissing;
	}
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *CodeGenerator::GetSourceFolder());

	// if we have less than 5 code files we are missing at least one
	if (FileNames.Num() < 5)
	{
		return ECompleteImportRequiredReason::FileMissing;
	}
	
	TArray<FAssetData> ArticyAssets;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*ArticyHelpers::ArticyGeneratedFolder), ArticyAssets, true);

	// check if all assets are actually valid (classes not found would result in a nullptr)
	for(FAssetData& Data : ArticyAssets)
	{
		UObject* Asset = Data.GetAsset();

		if(!Asset)
		{
			// if the asset exists but is invalid, the class is probably missing
			return ECompleteImportRequiredReason::FileMissing;
		}
	}

	// if we have less than 3 assets that means we have no package, no database or no global variables
	if(ArticyAssets.Num() < 3)
	{
		return ECompleteImportRequiredReason::ImportantAssetMissing;
	}

	return ECompleteImportRequiredReason::NotRequired;
}

void FArticyImporterModule::OnGeneratedCodeChanged(const TArray<FFileChangeData>& FileChanges)
{
	ECompleteImportRequiredReason RequiredReason = CheckIsCompleteReimportRequired();

	if(RequiredReason == ECompleteImportRequiredReason::FileMissing)
	{
		FText Message = FText::FromString(TEXT("It appears a generated code file is missing. Perform full reimport now?"));
		FText Title = FText::FromString(TEXT("Articy detected an error"));
		EAppReturnType::Type ReturnType = OpenMsgDlgInt(EAppMsgType::YesNo, Message, Title);

		if(ReturnType == EAppReturnType::Yes)
		{
			UArticyImportData* ImportData = nullptr;
			FArticyImporterFunctionLibrary::EnsureImportFile(&ImportData);
			FArticyImporterFunctionLibrary::ForceCompleteReimport(ImportData);
		}

	}	
}

void FArticyImporterModule::UnqueueImport()
{
	FEditorDelegates::EndPIE.Remove(QueuedImportHandle);
	QueuedImportHandle.Reset();
	bIsImportQueued = false;
}

void FArticyImporterModule::TriggerQueuedImport(bool b)
{
	UArticyImportData* ArticyImportData = nullptr;
	FArticyImporterFunctionLibrary::ReimportChanges(ArticyImportData);
	// important to unqueue in the end to reset the state
	UnqueueImport();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArticyImporterModule, ArticyImporter)