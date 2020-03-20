//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyEditorModule.h"
#include "ArticyPluginSettings.h"
#include "Customizations/ArticyPluginSettingsCustomization.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Dialogs/Dialogs.h"
#include <Widgets/SWindow.h>
#include "Widgets/Docking/SDockTab.h"

#include "ArticyEditorCommands.h"
#include "ArticyEditorFunctionLibrary.h"
#include "Editor.h"
#include "Customizations/ArticyRefCustomization.h"
#include "ArticyEditorStyle.h"
#include "CodeGeneration/CodeGenerator.h"
#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"
#include "LevelEditor.h"

DEFINE_LOG_CATEGORY(LogArticyEditor)

#define LOCTEXT_NAMESPACE "FArticyImporterModule"
static const FName ArticyWindowTabID("ArticyTab");

void FArticyEditorModule::StartupModule()
{
	RegisterPluginSettings();
	RegisterPluginCommands();
	RegisterConsoleCommands();
	RegisterDirectoryWatcher();
	RegisterArticyWindowTab();
	RegisterArticyToolbar();

	// register custom details for ArticyRef struct
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("ArticyRef", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FArticyRefCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	FArticyEditorStyle::Initialize();
}

void FArticyEditorModule::ShutdownModule()
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

void FArticyEditorModule::RegisterDirectoryWatcher()
{
	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>("DirectoryWatcher");
	DirectoryWatcherModule.Get()->RegisterDirectoryChangedCallback_Handle(CodeGenerator::GetSourceFolder(), IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FArticyEditorModule::OnGeneratedCodeChanged), GeneratedCodeWatcherHandle);
}

void FArticyEditorModule::SetCompleteReimportRequired()
{
	bIsCompleteReimportRequired = true;
}

void FArticyEditorModule::RegisterConsoleCommands()
{
	ConsoleCommands = new FArticyEditorConsoleCommands(*this);
}

void FArticyEditorModule::RegisterArticyToolbar()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{/*
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FStyleCheckerModule::AddMenuExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);*/
	}
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FArticyEditorModule::AddToolbarExtension));
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FArticyEditorModule::RegisterPluginCommands()
{
	FArticyEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(FArticyEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FArticyEditorModule::OpenArticyWindow),
		FCanExecuteAction());
}

void FArticyEditorModule::RegisterArticyWindowTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ArticyWindowTabID, FOnSpawnTab::CreateRaw(this, &FArticyEditorModule::OnSpawnArticyTab))
	.SetDisplayName(LOCTEXT("ArticyWindowTitle", "Articy Menu"))
	.SetIcon(FSlateIcon(FArticyEditorStyle::GetStyleSetName(), "ArticyImporter.ArticyImporter.16", "ArticyImporter.ArticyImporter.8"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FArticyEditorModule::RegisterPluginSettings() const
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

void FArticyEditorModule::UnregisterPluginSettings() const
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "ArticyImporter");
	}
}

bool FArticyEditorModule::IsImportQueued()
{
	return bIsImportQueued;
}

void FArticyEditorModule::QueueImport()
{
	bIsImportQueued = true;
	FOnMsgDlgResult OnDialogClosed;
	FText Message = LOCTEXT("ImportWhilePlaying", "To import articy:draft data, the play mode has to be quit. Import will begin after exiting play.");
	FText Title = LOCTEXT("ImportWhilePlaying_Title", "Import not possible");
	TSharedRef<SWindow> Window = OpenMsgDlgInt_NonModal(EAppMsgType::Ok, Message, Title, OnDialogClosed);
	Window->BringToFront(true);
	QueuedImportHandle = FEditorDelegates::EndPIE.AddRaw(this, &FArticyEditorModule::TriggerQueuedImport);
}

void FArticyEditorModule::OpenArticyWindow()
{
	FGlobalTabmanager::Get()->InvokeTab(ArticyWindowTabID);
}

EImportStatusValidity FArticyEditorModule::CheckImportStatusValidity() const
{
	UArticyImportData* ImportData = nullptr;
	FArticyEditorFunctionLibrary::EnsureImportFile(&ImportData);

	if (!ImportData)
	{
		return EImportStatusValidity::ImportDataAssetMissing;
	}
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *CodeGenerator::GetSourceFolder());

	// if we have less than 5 code files we are missing at least one
	if (FileNames.Num() < 5)
	{
		return EImportStatusValidity::FileMissing;
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
			return EImportStatusValidity::FileMissing;
		}
	}

	// if we have less than 3 assets that means we have no package, no database or no global variables
	if(ArticyAssets.Num() < 3)
	{
		return EImportStatusValidity::ImportantAssetMissing;
	}

	return EImportStatusValidity::Valid;
}

void FArticyEditorModule::OnGeneratedCodeChanged(const TArray<FFileChangeData>& FileChanges) const
{	
	const EImportStatusValidity Validity = CheckImportStatusValidity();

	// only check for missing files, as the code changes mid-import process too and we'd need to manage state if we wanted to check for assets as well when code changes
	if(Validity == EImportStatusValidity::FileMissing)
	{
		FText Message = FText::FromString(TEXT("It appears a generated code file is missing. Perform full reimport now?"));
		FText Title = FText::FromString(TEXT("Articy detected an error"));
		EAppReturnType::Type ReturnType = OpenMsgDlgInt(EAppMsgType::YesNo, Message, Title);

		if(ReturnType == EAppReturnType::Yes)
		{
			FArticyEditorFunctionLibrary::ForceCompleteReimport();
		}
	}
}

void FArticyEditorModule::UnqueueImport()
{
	FEditorDelegates::EndPIE.Remove(QueuedImportHandle);
	QueuedImportHandle.Reset();
	bIsImportQueued = false;
}

void FArticyEditorModule::TriggerQueuedImport(bool b)
{
	FArticyEditorFunctionLibrary::ReimportChanges();
	// important to unqueue in the end to reset the state
	UnqueueImport();
}

void FArticyEditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	//Builder.AddToolBarButton(FArticyEditorCommands::Get().OpenPluginWindow);
	Builder.AddToolBarButton(FArticyEditorCommands::Get().OpenPluginWindow, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FArticyEditorStyle::GetStyleSetName(), "ArticyImporter.ArticyImporter.40") );
}

TSharedRef<SDockTab> FArticyEditorModule::OnSpawnArticyTab(const FSpawnTabArgs& SpawnTabArgs) const
{
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			[
				SNew(SBox)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					//.AutoHeight()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.Text(LOCTEXT("ForceCompleteReimport", "Complete reimport"))
						.ToolTipText(LOCTEXT("ForceCompleteReimportTooltip", "Forces a complete reimport of articy draft data including code and asset generation."))
						.OnClicked_Lambda([]() -> FReply { FArticyEditorFunctionLibrary::ForceCompleteReimport(); return FReply::Handled(); })
					]
					+ SVerticalBox::Slot()
					//.AutoHeight()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.Text(LOCTEXT("ImportChanges", "Import Changes"))
						.ToolTipText(LOCTEXT("ImportChangesTooltip", "Imports only the changes from last import. This is usually quicker than a complete reimport."))
						.OnClicked_Lambda([]() -> FReply { FArticyEditorFunctionLibrary::ReimportChanges(); return FReply::Handled(); })
					]
					+ SVerticalBox::Slot()
					//.AutoHeight()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.Text(LOCTEXT("RegenerateAssets", "Regenerate assets"))
						.ToolTipText(LOCTEXT("RegenerateAssetsTooltip", "Regenerates all articy assets based on the currently generated code and the import data asset."))
						.OnClicked_Lambda([]() -> FReply { FArticyEditorFunctionLibrary::RegenerateAssets(); return FReply::Handled(); })
					]
				]
			]
		]
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Left)
		.Padding(5.f)
		[
			SNew(SImage)
			.Image(FArticyEditorStyle::Get().GetBrush("ArticyImporter.ArticySoftware.64"))
		]
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		.Padding(5.f)
		[
			SNew(SImage)
			.Image(FArticyEditorStyle::Get().GetBrush("ArticyImporter.ArticyDraftLogoText"))
		]
		// #TODO Additional widgets/controls go here
		/*+ SHorizontalBox::Slot()
		[
			SNew(SSpacer)
		]
		+ SHorizontalBox::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNullWidget::NullWidget
			]
		]*/
	];
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArticyEditorModule, ArticyEditor)