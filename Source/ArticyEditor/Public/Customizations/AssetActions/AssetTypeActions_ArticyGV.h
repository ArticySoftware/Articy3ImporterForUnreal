
#pragma once

#include "AssetTypeActions_Base.h"
#include "ArticyGlobalVariables.h"

class FAssetTypeActions_ArticyGV : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ArticyGV", "ArticyGV"); }

	virtual UClass* GetSupportedClass() const override
	{
		return UArticyGlobalVariables::StaticClass();
	}

	virtual FColor GetTypeColor() const override
	{
		return FColor(128, 128, 64);
	}

	virtual uint32 GetCategories() override { return EAssetTypeCategories::None; }
	
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,	TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
};

