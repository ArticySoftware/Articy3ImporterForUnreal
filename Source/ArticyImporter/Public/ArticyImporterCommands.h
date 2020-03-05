//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "SlateBasics.h"
#include "ArticyImporterStyle.h"

class FArticyImporterCommands : public TCommands<FArticyImporterCommands>
{
public:

	FArticyImporterCommands()
		: TCommands<FArticyImporterCommands>(TEXT("ArticyImporter"), NSLOCTEXT("Contexts", "ArticyImporter", "ArticyImporter Plugin"), NAME_None, FArticyImporterStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};