//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "ArticyAsset.h"
#include "FileMediaSource.h"

UTexture* UArticyAsset::LoadAsTexture() const
{
	return Cast<UTexture>(LoadAsset());
}

UTexture2D* UArticyAsset::LoadAsTexture2D() const
{
	return Cast<UTexture2D>(LoadAsset());
}

USoundWave* UArticyAsset::LoadAsSoundWave() const
{
	return Cast<USoundWave>(LoadAsset());
}

UFileMediaSource* UArticyAsset::LoadAsFileMediaSource() const
{
	return Cast<UFileMediaSource>(LoadAsset());
}

UObject* UArticyAsset::LoadAsset() const
{
	//if(!Asset.IsValid())
	{
		/*static TArray<FAssetData> assets;

		if(assets.Num() == 0)
		{
			static TWeakObjectPtr<UObjectLibrary> lib = nullptr;
			if(!lib.IsValid())
				lib = UObjectLibrary::CreateLibrary(UObject::StaticClass(), false, GIsEditor);

			lib->LoadAssetDataFromPath(ArticyHelpers::ArticyAssetsFolder);
			lib->GetAssetDataList(assets);
		}*/

		const auto folder = FPaths::GetPath(AssetRef);
		const auto filename = FPaths::GetBaseFilename(AssetRef); //without extension

		//construct the asset path like UE4 wants it
		auto path = ArticyHelpers::ArticyAssetsFolder / folder / filename;
		//UE_LOG(LogTemp, Warning, TEXT("Asset Path %s"), *path)
		Asset = ConstructorHelpersInternal::FindOrLoadObject<UObject>(path);
	}

	return Asset.Get();
}
