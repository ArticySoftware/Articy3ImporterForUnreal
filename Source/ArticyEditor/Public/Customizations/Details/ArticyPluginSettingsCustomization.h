//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#pragma once

#include "IDetailCustomization.h"

class FArticyPluginSettingsCustomization : public IDetailCustomization
{
public:
	FArticyPluginSettingsCustomization();

	~FArticyPluginSettingsCustomization();
	
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	// used to trigger a refresh on the custom UI
	void RefreshSettingsUI();
	FDelegateHandle RefreshHandle;
	
	IDetailLayoutBuilder* LayoutBuilder = nullptr;
};
