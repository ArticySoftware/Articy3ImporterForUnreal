//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "ArticyRuntimeModule.h"
#include "Internationalization/StringTableRegistry.h"

DEFINE_LOG_CATEGORY(LogArticyRuntime)

void FArticyRuntimeModule::StartupModule()
{
	LOCTABLE_FROMFILE_GAME("ArticyStrings", "ArticyStrings", "ArticyContent/Generated/ArticyStrings.csv");
}

void FArticyRuntimeModule::ShutdownModule()
{

}
	
IMPLEMENT_MODULE(FArticyRuntimeModule, ArticyRuntime)