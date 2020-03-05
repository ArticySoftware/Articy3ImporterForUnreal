//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "ArticyGeneratedModule.h"

DEFINE_LOG_CATEGORY(LogArticyGenerated)

bool FArticyGeneratedModule::IsGameModule() const
{
	return true;
}

bool FArticyGeneratedModule::SupportsDynamicReloading()
{
	return true;
}

void FArticyGeneratedModule::StartupModule()
{
	
}

void FArticyGeneratedModule::ShutdownModule()
{

}
	
IMPLEMENT_GAME_MODULE(FArticyGeneratedModule, ArticyGenerated)