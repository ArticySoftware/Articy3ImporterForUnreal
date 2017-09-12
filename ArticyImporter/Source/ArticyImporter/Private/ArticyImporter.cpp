//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyImporterPrivatePCH.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "ArticyImporterStyle.h"
#include "ArticyImporterCommands.h"

#include "LevelEditor.h"

#include "ArticyGlobalVariables.h"
#include "ArticyBaseTypes.h"
#include "ArticyHelpers.h"
#include "ArticyRef.h"

DEFINE_LOG_CATEGORY(LogArticyImporter)

static const FName ArticyImporterTabName("ArticyImporter");

#define LOCTEXT_NAMESPACE "FArticyImporterModule"

void FArticyImporterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	/*
	FArticyImporterStyle::Initialize();
	FArticyImporterStyle::ReloadTextures();

	FArticyImporterCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FArticyImporterCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FArticyImporterModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FArticyImporterModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FArticyImporterModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ArticyImporterTabName, FOnSpawnTab::CreateRaw(this, &FArticyImporterModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FArticyImporterTabTitle", "ArticyImporter"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	*/
}

void FArticyImporterModule::ShutdownModule()
{
	/*
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FArticyImporterStyle::Shutdown();

	FArticyImporterCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ArticyImporterTabName);*/
}
/*
TSharedRef<SDockTab> FArticyImporterModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FArticyImporterModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ArticyImporter.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FArticyImporterModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(ArticyImporterTabName);
}

void FArticyImporterModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FArticyImporterCommands::Get().OpenPluginWindow);
}

void FArticyImporterModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FArticyImporterCommands::Get().OpenPluginWindow);
}*/

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArticyImporterModule, ArticyImporter)