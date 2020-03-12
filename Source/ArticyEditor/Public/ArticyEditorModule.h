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
enum ECompleteImportRequiredReason;

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
	
	/* Plugin settings menu */
	void RegisterPluginSettings();
	void UnregisterPluginSettings();

	void QueueImport();
	bool IsImportQueued();

	void SetCompleteReimportRequired();

	FOnImportFinished OnImportFinished;
	FOnCompilationFinished OnCompilationFinished;

private:
	ECompleteImportRequiredReason CheckIsCompleteReimportRequired() const;
	void OnGeneratedCodeChanged(const TArray<struct FFileChangeData>& FileChanges);

	void UnqueueImport();
	void TriggerQueuedImport(bool b);
private:

	bool bIsCompleteReimportRequired = false;
	bool bIsImportQueued = false;
	FDelegateHandle QueuedImportHandle;
	FDelegateHandle GeneratedCodeWatcherHandle;
	FArticyEditorConsoleCommands* ConsoleCommands = nullptr;
};

enum ECompleteImportRequiredReason
{
	NotRequired,
	ImportantAssetMissing,
	FileMissing,
	ImportDataAssetMissing
};