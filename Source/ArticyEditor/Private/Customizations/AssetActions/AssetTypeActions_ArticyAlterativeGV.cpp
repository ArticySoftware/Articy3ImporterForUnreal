//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//
#include "AssetTypeActions_ArticyAlterativeGV.h"
#include "ArticyAlternativeGlobalVariables.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

bool FAssetTypeActions_ArticyAlterativeGV::CanFilter()
{
	return true;
}

uint32 FAssetTypeActions_ArticyAlterativeGV::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

FText FAssetTypeActions_ArticyAlterativeGV::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AlternativeGlobalVariables", "Alternative Articy Global Variables");
}

UClass* FAssetTypeActions_ArticyAlterativeGV::GetSupportedClass() const
{
	return UArticyAlternativeGlobalVariables::StaticClass();
}


FColor FAssetTypeActions_ArticyAlterativeGV::GetTypeColor() const
{
	return FColor(128, 128, 64);
}

#undef LOCTEXT_NAMESPACE
