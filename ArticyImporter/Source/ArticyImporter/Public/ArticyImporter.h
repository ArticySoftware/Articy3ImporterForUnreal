//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Misc/CompilationResult.h"
#include "ArticyJSONFactory.h"
#include "ArticyImporterHelpers.h"
#include "IDelegateInstance.h"
#include "ArticyRef.h"
#include "ArticyImporterConsoleCommands.h"


DECLARE_LOG_CATEGORY_EXTERN(LogArticyImporter, Log, All)

DECLARE_MULTICAST_DELEGATE(FOnImportFinished);
DECLARE_MULTICAST_DELEGATE(FOnCompilationFinished);

class FToolBarBuilder;
class FMenuBuilder;

class FArticyImporterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FArticyImporterModule& Get()
	{
		static const FName ModuleName = "ArticyImporter";
		return FModuleManager::LoadModuleChecked<FArticyImporterModule>(ModuleName);
	}


	void RegisterConsoleCommands();
	
	/* Plugin settings menu */
	void RegisterPluginSettings();
	void UnregisterPluginSettings();

	void QueueImport();
	bool IsImportQueued();

	FOnImportFinished OnImportFinished;
	FOnCompilationFinished OnCompilationFinished;

private:

	void UnqueueImport();
	void TriggerQueuedImport(bool b);
private:

	bool bIsImportQueued = false;
	FDelegateHandle QueuedImportHandle;
	FArticyImporterConsoleCommands* ConsoleCommands;
};
