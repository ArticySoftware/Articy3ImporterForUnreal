//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//
#pragma once

#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArticyEditor, Log, All)


class FArticyEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
