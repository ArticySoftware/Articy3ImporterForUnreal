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


/**
 *  REFERENCE: SAssetViewItem, which is the normal asset selection widget in a picker
 */
class ARTICYIMPORTER_API SDialogueEntityTileView : public SCompoundWidget
{
public:	
	SLATE_BEGIN_ARGS(SDialogueEntityTileView) 
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
	TSharedPtr<SImage> EntityImage;
	TSharedPtr<STextBlock> EntityNameTextBlock;
	TSharedPtr<SBorder> WidgetContainerBorder;

	TSharedPtr<FSlateBrush> ArticyObjectImageBrush;
	TSharedPtr<FTextBlockStyle> EntityNameTextStyle;
	int32 ThumbnailSize;
	int32 ThumbnailPadding;

private:

	
	FText OnGetEntityName() const;
	const FSlateBrush* OnGetEntityImage() const;
public:
};
