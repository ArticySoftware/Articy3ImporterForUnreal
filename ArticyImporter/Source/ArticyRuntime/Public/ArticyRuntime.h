//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Engine.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArticyRuntime, Log, All)

class FToolBarBuilder;
class FMenuBuilder;

class FArticyRuntimeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
};