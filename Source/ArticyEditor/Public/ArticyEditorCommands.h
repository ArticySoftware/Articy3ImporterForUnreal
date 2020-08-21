//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "ArticyEditorStyle.h"
#include "Framework/Commands/Commands.h"

class FArticyEditorCommands : public TCommands<FArticyEditorCommands>
{
public:

	FArticyEditorCommands()
		: TCommands<FArticyEditorCommands>(TEXT("ArticyImporter"), NSLOCTEXT("Contexts", "ArticyImporter", "ArticyImporter Plugin"), NAME_None, FArticyEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenArticyImporter;
	TSharedPtr<FUICommandInfo> OpenArticyGVDebugger;
};
