//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//
#pragma once

#include "Modules/ModuleInterface.h"

// compile time messages
#define PRAGMA_STR1(x)  #x
#define PRAGMA_STR2(x)  PRAGMA_STR1 (x)
#define NOTE(x)  message (__FILE__ "(" PRAGMA_STR2(__LINE__) ") : -NOTE- " #x)
#define NOTE_wARG(x,y)  message (__FILE__ "(" PRAGMA_STR2(__LINE__) ") : -NOTE- " #x PRAGMA_STR2(y))

// ArticyEditor log category
DECLARE_LOG_CATEGORY_EXTERN(LogArticyEditor, Log, All)

class FArticyEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
