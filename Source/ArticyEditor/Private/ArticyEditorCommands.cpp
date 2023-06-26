//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "ArticyEditorCommands.h"

#define LOCTEXT_NAMESPACE "FArticyImporterModule"

void FArticyEditorCommands::RegisterCommands()
{
	// FInputChord can be modified for default shortcuts, but they don't seem to work inside the level editor by default
	UI_COMMAND(OpenArticyImporter, "Articy X Importer", "Bring up ArticyImporter window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(OpenArticyGVDebugger, "Articy GV Debugger", "Bring up the runtime Global Variables debugger", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
