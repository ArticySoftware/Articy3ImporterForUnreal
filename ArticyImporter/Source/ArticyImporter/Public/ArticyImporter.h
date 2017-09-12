//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "CompilationResult.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArticyImporter, Log, All)

class FToolBarBuilder;
class FMenuBuilder;

class FArticyImporterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	//void PluginButtonClicked();
	
private:
	//void AddToolbarExtension(FToolBarBuilder& Builder);
	//void AddMenuExtension(FMenuBuilder& Builder);

	//TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	//TSharedPtr<class FUICommandList> PluginCommands;
};