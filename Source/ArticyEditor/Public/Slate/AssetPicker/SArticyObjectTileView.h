//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SCompoundWidget.h"
#include "ArticyObject.h"
#include "Widgets/Layout/SBorder.h"
#include "ArticyBaseTypes.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/Commands/UIAction.h"

DECLARE_DELEGATE_OneParam(FOnArticyIdChanged, const FArticyId&);

/**
 *  REFERENCE: SAssetViewItem, which is the normal asset selection widget in a picker
 */
class ARTICYEDITOR_API SArticyObjectTileView : public SCompoundWidget
{
public:
  SLATE_BEGIN_ARGS(SArticyObjectTileView) 
		: _LabelVisibility(EVisibility::Visible)
		, _OnMouseDoubleClick()
		, _ThumbnailSize(FVector2D(48.f, 48.f))	
		, _ThumbnailPadding(FMargin(2.f))
		, _bIsReadOnly(false)
	{}
		SLATE_ATTRIBUTE(FArticyId, ArticyIdToDisplay)
		SLATE_ATTRIBUTE(EVisibility, LabelVisibility)
	    SLATE_EVENT(FPointerEventHandler, OnMouseDoubleClick)
		SLATE_EVENT(FOnArticyIdChanged, OnArticyIdChanged)
		SLATE_ARGUMENT(FVector2D, ThumbnailSize)
		SLATE_ARGUMENT(FMargin, ThumbnailPadding)
		SLATE_ATTRIBUTE(bool, bIsReadOnly)
		SLATE_ARGUMENT(FUIAction, CopyAction)
		SLATE_ARGUMENT(FUIAction, PasteAction)
	SLATE_END_ARGS()

/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void Update(const FArticyId& NewArticyId);

private:
	TAttribute<FArticyId> ArticyIdToDisplay;
	FOnArticyIdChanged OnArticyIdChanged;
	FOnArticyIdChanged OnArticyIdPasted;
	FPointerEventHandler OnMouseDoubleClick;
	TAttribute<EVisibility> LabelVisibility;
	FVector2D ThumbnailSize;
	FMargin ThumbnailPadding;
	TAttribute<bool> bIsReadOnly;
	FUIAction CopyAction;
	FUIAction PasteAction;
	
	mutable FArticyId CachedArticyId;
	
	mutable TWeakObjectPtr<UArticyObject> CachedArticyObject;
	
	TSharedPtr<SImage> PreviewImage;
	TSharedPtr<STextBlock> DisplayNameTextBlock;
	TSharedPtr<SBorder> WidgetContainerBorder;

	mutable FSlateBrush PreviewBrush;
	const FSlateBrush* TypeImage = nullptr;
	TSharedPtr<FTextBlockStyle> EntityNameTextStyle;
	bool bHasPreviewImage = false;

private:
	void OnContextMenuOpening(class FMenuBuilder& Builder) const;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	void UpdateWidget();
	
	virtual FText OnGetEntityName() const;
	virtual const FSlateBrush* OnGetEntityImage() const;
	EVisibility OnHasPreviewImage() const;
	virtual FSlateColor OnGetArticyObjectColor() const;
	const FSlateBrush* GetTypeImage() const;
};
