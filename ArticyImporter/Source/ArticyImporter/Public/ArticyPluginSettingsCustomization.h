//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include "IDetailCustomization.h"

class FArticyPluginSettingsCustomization : public IDetailCustomization
{
public:
	FArticyPluginSettingsCustomization();

	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	// used to trigger a refresh on the custom UI
	void RefreshSettingsUI();
	FDelegateHandle RefreshHandle;
	
	IDetailLayoutBuilder* LayoutBuilder;
};
