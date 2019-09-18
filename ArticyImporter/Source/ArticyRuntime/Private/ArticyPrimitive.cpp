//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyPrimitive.h"
#include "ArticyDatabase.h"

UArticyPrimitive* UArticyPrimitive::FindAsset(const FArticyId& Id)
{
	//UArticyDatabase::LoadAllObjects();

	//get the reference object by Id
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByClass(StaticClass()->GetFName(), AssetData, true);

	for(const auto asset : AssetData)
	{
		const auto obj = Cast<UArticyPrimitive>(asset.GetAsset());
		if(obj && obj->WasLoaded() && obj->GetId() == Id)
			return obj;
	}

	return nullptr;
}
