//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyEditorModule.h"
#include "ArticyPluginSettings.h"
#include "ArticyEditorCommands.h"
#include "ArticyEditorFunctionLibrary.h"
#include "ArticyEditorStyle.h"
#include "ArticyFlowClasses.h"
#include "CodeGeneration/CodeGenerator.h"
#include "Customizations/ArticyRefWidgetCustomizations/DefaultArticyRefWidgetCustomizations.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Misc/MessageDialog.h"
#include "Dialogs/Dialogs.h"
#include <Widgets/SWindow.h>
#include "AssetToolsModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Editor.h"
#include "DirectoryWatcherModule.h"
#include "HAL/FileManager.h"
#include "Widgets/Images/SImage.h"
#include "IDirectoryWatcher.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"
#include "Customizations/ArticyPinFactory.h"
#include "Customizations/AssetActions/AssetTypeActions_ArticyGV.h"
#include "Customizations/Details/ArticyGVCustomization.h"
#include "Customizations/Details/ArticyPluginSettingsCustomization.h"
#include "Customizations/Details/ArticyRefCustomization.h"

DEFINE_LOG_CATEGORY(LogArticyEditor)

#define LOCTEXT_NAMESPACE "FArticyImporterModule"
static const FName ArticyWindowTabID("ArticyWindowTab");
static const FName ArticyGVDebuggerTabID("ArticyGVDebuggerTab");

void FArticyEditorModule::StartupModule()
{
	CustomizationManager = MakeShareable(new FArticyEditorCustomizationManager);
	
	RegisterArticyToolbar();
	RegisterAssetTypeActions();
	RegisterConsoleCommands();
	RegisterDefaultArticyRefWidgetExtensions();
	RegisterDetailCustomizations();
	RegisterGraphPinFactory();
	RegisterPluginSettings();
	RegisterPluginCommands();
	// directory watcher has to be changed or removed as the results aren't quite deterministic
	//RegisterDirectoryWatcher();
	RegisterToolTabs();
	
	FArticyEditorStyle::Initialize();
}

void FArticyEditorModule::ShutdownModule()
{
	if (UObjectInitialized())
	{
		UnregisterDefaultArticyRefWidgetExtensions();
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

void FArticyEditorModule::RegisterGraphPinFactory() const
{
	TSharedPtr<FArticyRefPinFactory> ArticyRefPinFactory = MakeShareable(new FArticyRefPinFactory);
	FEdGraphUtilities::RegisterVisualPinFactory(ArticyRefPinFactory);
}

void FArticyEditorModule::RegisterConsoleCommands()
{
	ConsoleCommands = new FArticyEditorConsoleCommands(*this);
}

void FArticyEditorModule::RegisterDefaultArticyRefWidgetExtensions() const
{
	// this registers the articy button extension for all UArticyObjects.
	GetCustomizationManager()->RegisterArticyRefWidgetCustomizationFactory(FOnCreateArticyRefWidgetCustomizationFactory::CreateLambda([]()
	{
		return MakeShared<FArticyButtonCustomizationFactory>();
	}));
}

void FArticyEditorModule::RegisterDetailCustomizations() const
{
	// register custom details for ArticyRef struct
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.RegisterCustomPropertyTypeLayout("ArticyRef", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FArticyRefCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("ArticyPluginSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FArticyPluginSettingsCustomization::MakeInstance));
	PropertyModule.RegisterCustomClassLayout("ArticyGlobalVariables", FOnGetDetailCustomizationInstance::CreateStatic(&FArticyGVCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

TArray<UArticyPackage*> FArticyEditorModule::GetPackagesSlow()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> PackageData;
	AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetFName(), PackageData);

	TArray<UArticyPackage*> Packages;
	for(FAssetData& Data : PackageData)
	{
		Packages.Add(Cast<UArticyPackage>(Data.GetAsset()));
	}

	return Packages;
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

void FArticyEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_ArticyGV()));
}

void FArticyEditorModule::RegisterPluginCommands()
{
	FArticyEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(FArticyEditorCommands::Get().OpenArticyImporter,
		FExecuteAction::CreateRaw(this, &FArticyEditorModule::OpenArticyWindow),
		FCanExecuteAction());

	PluginCommands->MapAction(FArticyEditorCommands::Get().OpenArticyGVDebugger,
		FExecuteAction::CreateRaw(this, &FArticyEditorModule::OpenArticyGVDebugger),
		FCanExecuteAction());
}

void FArticyEditorModule::RegisterToolTabs()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ArticyWindowTabID, FOnSpawnTab::CreateRaw(this, &FArticyEditorModule::OnSpawnArticyMenuTab))
	.SetDisplayName(LOCTEXT("ArticyWindowTitle", "Articy Menu"))
	.SetIcon(FSlateIcon(FArticyEditorStyle::GetStyleSetName(), "ArticyImporter.ArticyImporter.16", "ArticyImporter.ArticyImporter.8"))
	.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ArticyGVDebuggerTabID, FOnSpawnTab::CreateRaw(this, &FArticyEditorModule::OnSpawnArticyGVDebuggerTab))
		.SetDisplayName(LOCTEXT("ArticyGVDebuggerTitle", "Articy GV Debugger"))
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
}

void FArticyEditorModule::UnregisterDefaultArticyRefWidgetExtensions() const
{
	for(const IArticyRefWidgetCustomizationFactory* DefaultFactory : DefaultArticyRefWidgetCustomizationFactories)
	{
		GetCustomizationManager()->UnregisterArticyRefWidgetCustomizationFactory(DefaultFactory);
	}
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

void FArticyEditorModule::OpenArticyGVDebugger()
{
	FGlobalTabmanager::Get()->InvokeTab(ArticyGVDebuggerTabID);
}

EImportStatusValidity FArticyEditorModule::CheckImportStatusValidity() const
{
	UArticyImportData* ImportData = nullptr;
	FArticyEditorFunctionLibrary::EnsureImportDataAsset(&ImportData);

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
#if ENGINE_MINOR_VERSION <= 24
		EAppReturnType::Type ReturnType = OpenMsgDlgInt(EAppMsgType::YesNo, Message, Title);
#else
		EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNo, Message, &Title);
#endif

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
	Builder.AddComboButton(FUIAction(), FOnGetContent::CreateRaw(this, &FArticyEditorModule::OnGenerateArticyToolsMenu), FText::FromString(TEXT("Articy Tools")), TAttribute<FText>(), FSlateIcon(FArticyEditorStyle::GetStyleSetName(), "ArticyImporter.ArticyImporter.40") );
	//Builder.AddToolBarButton(FArticyEditorCommands::Get().OpenPluginWindow, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FArticyEditorStyle::GetStyleSetName(), "ArticyImporter.ArticyImporter.40") );
}

TSharedRef<SWidget> FArticyEditorModule::OnGenerateArticyToolsMenu() const
{
	FMenuBuilder MenuBuilder(true, PluginCommands);

	MenuBuilder.BeginSection("ArticyTools", LOCTEXT("ArticyTools", "Articy Tools"));
	MenuBuilder.AddMenuEntry(FArticyEditorCommands::Get().OpenArticyImporter);
	MenuBuilder.AddMenuEntry(FArticyEditorCommands::Get().OpenArticyGVDebugger);
	MenuBuilder.EndSection();
	
	return MenuBuilder.MakeWidget();
}

TSharedRef<SDockTab> FArticyEditorModule::OnSpawnArticyMenuTab(const FSpawnTabArgs& SpawnTabArgs) const
{
	float ButtonWidth = 333.f / 1.3f;
	float ButtonHeight = 101.f / 1.3f;
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Center)
			.Padding(10.f)
			[
				SNew(SImage)
				.Image(FArticyEditorStyle::Get().GetBrush("ArticyImporter.Window.ImporterLogo"))
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Center)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(ButtonWidth)
					.HeightOverride(ButtonHeight)
					[
						SNew(SButton)
						.ButtonStyle(FArticyEditorStyle::Get(), "ArticyImporter.Button.FullReimport")
						.ToolTipText(LOCTEXT("ForceCompleteReimportTooltip", "Forces a complete reimport of articy draft data including code and asset generation."))
						.OnClicked_Lambda([]() -> FReply { FArticyEditorFunctionLibrary::ForceCompleteReimport(); return FReply::Handled(); })
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(ButtonWidth)
					.HeightOverride(ButtonHeight)
					[
						SNew(SButton)
						.ButtonStyle(FArticyEditorStyle::Get(), "ArticyImporter.Button.ImportChanges")
						.ToolTipText(LOCTEXT("ImportChangesTooltip", "Imports only the changes from last import. This is usually quicker than a complete reimport."))
						.OnClicked_Lambda([]() -> FReply { FArticyEditorFunctionLibrary::ReimportChanges(); return FReply::Handled(); })
					]
				]
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(ButtonWidth)
					.HeightOverride(ButtonHeight)
					[
						SNew(SButton)
						.ButtonStyle(FArticyEditorStyle::Get(), "ArticyImporter.Button.RegenerateAssets")
						.ToolTipText(LOCTEXT("RegenerateAssetsTooltip", "Regenerates all articy assets based on the currently generated code and the import data asset."))
						.OnClicked_Lambda([]() -> FReply { FArticyEditorFunctionLibrary::RegenerateAssets(); return FReply::Handled(); })
					]
				]
			]
		]
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.HAlign(HAlign_Right)
		.Padding(5.f)
		[
			SNew(SImage)
			.Image(FArticyEditorStyle::Get().GetBrush("ArticyImporter.Window.ArticyLogo"))
		]
	];
}

TSharedRef<SDockTab> FArticyEditorModule::OnSpawnArticyGVDebuggerTab(const FSpawnTabArgs& SpawnTabArgs) const
{
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SArticyGlobalVariablesRuntimeDebugger).bInitiallyCollapsed(true)
	];
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FArticyEditorModule, ArticyEditor)