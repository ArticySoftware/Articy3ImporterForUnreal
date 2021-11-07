//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//
#pragma once

#include "AssetTypeActions_Base.h"

/**
 * Implements an action for UTextAsset assets.
 */
class FAssetTypeActions_ArticyAlterativeGV
	: public FAssetTypeActions_Base
{
public:
	virtual bool CanFilter() override;
	virtual uint32 GetCategories() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
};