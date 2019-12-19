// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <SlateImageBrush.h>
#include <PropertyHandle.h>
#include "ArticyObjectWithPreviewImage.h"
#include <SlateBrush.h>
#include <SCompoundWidget.h>
#include "ArticyObject.h"
#include <SBorder.h>
#include "UserInterfaceHelperFunctions.h"


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

	SLATE_ARGUMENT(TWeakObjectPtr<UArticyObject>, ObjectToDisplay)
	SLATE_ARGUMENT(int32, ThumbnailSize)
	SLATE_ARGUMENT(int32, ThumbnailPadding)

	SLATE_END_ARGS()

/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<UArticyObject> ObjectToDisplay;
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
	FText OnGetEntityName() const;
	const FSlateBrush* OnGetEntityImage() const;
	EVisibility OnHasPreviewImage() const;
	const FSlateBrush* OnGetTypeImage(UserInterfaceHelperFunctions::EImageSize SizeOverride = UserInterfaceHelperFunctions::Medium) const;

};
