//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "PackagesGenerator.h"
#include "ArticyImportData.h"

void PackagesGenerator::GenerateAssets(UArticyImportData* Data)
{
	auto pack = Data->GetPackageDefs();
	pack.GenerateAssets(Data);
}
