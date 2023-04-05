//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Slate/GV/SArticyGlobalVariablesDebugger.h"
#include "DetailLayoutBuilder.h"

/** This class is not really actively used, as we use the GVEditor to overwrite the asset editor completely, and the GV runtime debugger.
 *	We use it just in case someone externally summons a details tab. There will be a useless default seach bar and a custom one using the details panel. */
class FArticyGVCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
