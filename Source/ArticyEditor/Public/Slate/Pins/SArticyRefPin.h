//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "ArticyBaseTypes.h"
#include <SGraphPin.h>
#include "ArticyRef.h"
// Ref: SGraphPinColor
class SArticyRefPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SArticyRefPin) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* GraphPin);

	// SGraphPin Interface
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	virtual EVisibility GetDefaultValueVisibility() const override;

private:
	/** Serves as cache of the currently selected value.
	 * The actual value is saved in the DefaultValue of the parent UEdGraphPin 
	 */
	FArticyRef ArticyRef = FArticyRef();
private:
	FArticyRef GetArticyRef() const;
	void OnArticyRefChanged(const FArticyRef& NewArticyRef);
};
