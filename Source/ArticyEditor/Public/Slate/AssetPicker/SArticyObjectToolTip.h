//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include <Widgets/SToolTip.h>
#include "Widgets/SBoxPanel.h"
#include "ArticyObject.h"

// Reference: AssetViewWidgets.h: CreateToolTipWidget

class SArticyObjectTileView;

class SArticyObjectToolTip : public SToolTip
{
public:
	SLATE_BEGIN_ARGS(SArticyObjectToolTip)		
	{}
		SLATE_ATTRIBUTE(FArticyId, ObjectToDisplay)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void OnOpening() override;
	virtual void OnClosed() override;

protected:
	void UpdateWidget();

	TAttribute<FArticyId> ArticyIdAttribute;
	mutable FArticyId CachedArticyId;
	mutable TWeakObjectPtr<UArticyObject> CachedArticyObject;
	FSlateBrush TooltipBrush;

	TSharedRef<SWidget> CreateTooltipWidget(FText NameText, TSharedRef<SVerticalBox> InfoBox);
	TSharedRef<SWidget> CreateToolTipContent();
	TSharedRef<SWidget> CreateContentForEmpty();
	/** Adds key value type content to the tooltip box (i.e. "Text: This Is Sample Text" */
	void AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, const FText& Value, bool bImportant) const;
private:
	const FSlateBrush* GetTooltipImage() const;
};
