//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "PackagesGenerator.h"
#include "ArticyImportData.h"

#define LOCTEXT_NAMESPACE "PackagesGenerator"

void PackagesGenerator::GenerateAssets(UArticyImportData* Data)
{
	// generate new articy objects
	const auto ArticyPackageDefs = Data->GetPackageDefs();
	ArticyPackageDefs.GenerateAssets(Data);
}

#undef LOCTEXT_NAMESPACE
