//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Delegates/IDelegateInstance.h"
#include "ArticyEditorConsoleCommands.h"


DECLARE_LOG_CATEGORY_EXTERN(LogArticyEditor, Log, All)

DECLARE_MULTICAST_DELEGATE(FOnImportFinished);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCompilationFinished, UArticyImportData*);

class FToolBarBuilder;
class FMenuBuilder;
enum EImportStatusValidity;

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

	void RegisterDirectoryWatcher();
	void RegisterConsoleCommands();
	void RegisterPluginCommands();
	void RegisterArticyWindowTab();
	void RegisterArticyToolbar();


	/* Plugin settings menu */
	void RegisterPluginSettings() const;
	void UnregisterPluginSettings() const;

	void QueueImport();
	bool IsImportQueued();

	FOnImportFinished OnImportFinished;
	FOnCompilationFinished OnCompilationFinished;

private:
	void OpenArticyWindow();
	EImportStatusValidity CheckImportStatusValidity() const;
	void OnGeneratedCodeChanged(const TArray<struct FFileChangeData>& FileChanges) const;

	void UnqueueImport();
	void TriggerQueuedImport(bool b);
	
	void AddToolbarExtension(FToolBarBuilder& Builder);
	TSharedRef<class SDockTab> OnSpawnArticyTab(const class FSpawnTabArgs& SpawnTabArgs) const;
	
private:
	bool bIsImportQueued = false;
	FDelegateHandle QueuedImportHandle;
	FDelegateHandle GeneratedCodeWatcherHandle;
	FArticyEditorConsoleCommands* ConsoleCommands = nullptr;
	TSharedPtr<FUICommandList> PluginCommands;
};

enum EImportStatusValidity
{
	Valid,
	ImportantAssetMissing,
	FileMissing,
	ImportDataAssetMissing
};