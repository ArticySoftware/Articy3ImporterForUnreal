//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include <EdGraphUtilities.h>
#include "Slate/Pins/SArticyRefPin.h"
#include "Slate/Pins/SArticyIdPin.h"

class FArticyRefPinFactory : public FGraphPanelPinFactory
{
public:
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override
	{
		if(Pin->PinType.PinSubCategoryObject.IsValid() && Pin->PinType.PinSubCategoryObject.Get() == FArticyRef::StaticStruct())
		{
			return SNew(SArticyRefPin, Pin);
		}
		if(Pin->PinType.PinSubCategoryObject.IsValid() && Pin->PinType.PinSubCategoryObject.Get() == FArticyId::StaticStruct()) 
		{
			return SNew(SArticyIdPin, Pin);
		}

		return nullptr;
	}
};