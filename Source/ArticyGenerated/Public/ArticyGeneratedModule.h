//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

//#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArticyGenerated, Log, All)

class FToolBarBuilder;
class FMenuBuilder;

class FArticyGeneratedModule : public IModuleInterface
{
public:
	virtual bool IsGameModule() const override;
	virtual bool SupportsDynamicReloading() override;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	
};