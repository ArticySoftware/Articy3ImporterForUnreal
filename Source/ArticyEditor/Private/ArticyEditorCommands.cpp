//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "ArticyEditorCommands.h"

#define LOCTEXT_NAMESPACE "FArticyImporterModule"

void FArticyEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Articy Importer", "Bring up ArticyImporter window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
