//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/ArticyObjectWithPreviewImage.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SCompoundWidget.h"
#include "ArticyObject.h"
#include "Widgets/Layout/SBorder.h"
#include "ArticyBaseTypes.h"
#include "Slate/UserInterfaceHelperFunctions.h"


/**
 *  REFERENCE: SAssetViewItem, which is the normal asset selection widget in a picker
 */
class ARTICYIMPORTER_API SArticyObjectTileView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArticyObjectTileView) 
		: _ThumbnailSize(48.f)	
		, _ThumbnailPadding(4.f)
	{}

		SLATE_ATTRIBUTE(FArticyId, ObjectToDisplay)
		SLATE_ARGUMENT(int32, ThumbnailSize)
		SLATE_ARGUMENT(int32, ThumbnailPadding)

	SLATE_END_ARGS()

/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	TAttribute<FArticyId> ArticyIdAttribute;
	mutable FArticyId CachedArticyId;
	mutable TWeakObjectPtr<UArticyObject> CachedArticyObject;
	TSharedPtr<SImage> PreviewImage;
	TSharedPtr<STextBlock> DisplayNameTextBlock;
	TSharedPtr<SBorder> WidgetContainerBorder;

	mutable FSlateBrush PreviewBrush;
	const FSlateBrush* TypeImage = nullptr;
	TSharedPtr<FTextBlockStyle> EntityNameTextStyle;
	int32 ThumbnailSize;
	int32 ThumbnailPadding;

	bool bHasPreviewImage = false;
	bool bHasColor = false;
	// default articy color for objects that do not have a color but still need a border
	FSlateColor ArticyObjectColor = FLinearColor(0.577, 0.76, 0.799);
	
private:
	void UpdateDisplayedArticyObject();

	FText OnGetEntityName() const;
	const FSlateBrush* OnGetEntityImage() const;
	EVisibility OnHasPreviewImage() const;
	FSlateColor OnGetArticyObjectColor() const;
	const FSlateBrush* GetTypeImage(UserInterfaceHelperFunctions::EImageSize SizeOverride = UserInterfaceHelperFunctions::Medium) const;
};
