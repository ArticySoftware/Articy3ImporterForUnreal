//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include <Widgets/SToolTip.h>
#include "ArticyObject.h"

// Reference: AssetViewWidgets.h: CreateToolTipWidget

class SArticyObjectTileView;

class SArticyObjectToolTip : public SToolTip
{
public:
	SLATE_BEGIN_ARGS(SArticyObjectToolTip)
		: _ObjectToDisplay(nullptr)
	{ }

		SLATE_ARGUMENT(TWeakObjectPtr<UArticyObject>, ObjectToDisplay)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void OnOpening() override;
	virtual void OnClosed() override;

protected:

	TWeakObjectPtr<UArticyObject> ObjectToDisplay;
	FSlateBrush TooltipBrush;

	TSharedRef<SWidget> CreateToolTipContent();
	/** Adds key value type content to the tooltip box (i.e. "Text: This Is Sample Text" */
	void AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, const FText& Value, bool bImportant) const;

private:
	const FSlateBrush* GetTooltipImage() const;
};
