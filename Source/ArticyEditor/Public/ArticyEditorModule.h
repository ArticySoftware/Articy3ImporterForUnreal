//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Delegates/IDelegateInstance.h"
#include "ArticyEditorConsoleCommands.h"
#include "Customizations/ArticyEditorCustomizationManager.h"
#include "Framework/Commands/UICommandList.h"
#include "Slate/SArticyRefProperty.h"


DECLARE_LOG_CATEGORY_EXTERN(LogArticyEditor, Log, All)

DECLARE_MULTICAST_DELEGATE(FOnImportFinished);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCompilationFinished, UArticyImportData*);

class FToolBarBuilder;
class FMenuBuilder;

enum EImportStatusValidity
{
	Valid,
	ImportantAssetMissing,
	FileMissing,
	ImportDataAssetMissing
};

class FArticyEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FArticyEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FArticyEditorModule>(TEXT("ArticyEditor"));
	}

	TSharedPtr<FArticyEditorCustomizationManager> GetCustomizationManager() const { return CustomizationManager; }
	
	void RegisterArticyToolbar();
	void RegisterConsoleCommands();
	/** Registers all default widget extensions. As of this point, the articy button */
	void RegisterDefaultArticyRefWidgetExtensions() const;
	void RegisterDetailCustomizations() const;
	void RegisterDirectoryWatcher();
	void RegisterPluginCommands();
	void RegisterPluginSettings() const;
	void RegisterToolTabs();

	void UnregisterPluginSettings() const;
	void UnregisterDefaultArticyRefWidgetExtensions() const;
	
	void QueueImport();
	bool IsImportQueued();

	/** Delegate to bind custom logic you want to perform after the import has successfully finished */
	FOnImportFinished OnImportFinished;
	FOnCompilationFinished OnCompilationFinished;

private:
	void OpenArticyWindow();
	void OpenArticyGVDebugger();

	EImportStatusValidity CheckImportStatusValidity() const;
	void OnGeneratedCodeChanged(const TArray<struct FFileChangeData>& FileChanges) const;

	void UnqueueImport();
	void TriggerQueuedImport(bool b);
	
	void AddToolbarExtension(FToolBarBuilder& Builder);
	TSharedRef<SWidget> OnGenerateArticyToolsMenu() const;
	TSharedRef<class SDockTab> OnSpawnArticyMenuTab(const class FSpawnTabArgs& SpawnTabArgs) const;
	TSharedRef<class SDockTab> OnSpawnArticyGVDebuggerTab(const class FSpawnTabArgs& SpawnTabArgs) const;
	
private:
	bool bIsImportQueued = false;
	FDelegateHandle QueuedImportHandle;
	FDelegateHandle GeneratedCodeWatcherHandle;
	FArticyEditorConsoleCommands* ConsoleCommands = nullptr;
	TSharedPtr<FUICommandList> PluginCommands;
	/** The CustomizationManager registers and owns all customization factories */
	TSharedPtr<FArticyEditorCustomizationManager> CustomizationManager = nullptr;

	/** The CustomizationManager has ownership of the factories. These here are cached for removal at shutdown */
	TArray<const IArticyRefWidgetCustomizationFactory*> DefaultArticyRefWidgetCustomizationFactories;
};
