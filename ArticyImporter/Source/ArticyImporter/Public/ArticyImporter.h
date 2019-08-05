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


DECLARE_LOG_CATEGORY_EXTERN(LogArticyImporter, Log, All)

class FToolBarBuilder;
class FMenuBuilder;

class FArticyImporterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/* Plugin settings menu */
	void RegisterPluginSettings();
	void UnregisterPluginSettings();

	void QueueImport();
	bool IsImportQueued();

private:

	void UnqueueImport();
	void TriggerQueuedImport(bool b);

private:

	bool bIsImportQueued = false;
	FDelegateHandle QueuedImportHandle;
};
