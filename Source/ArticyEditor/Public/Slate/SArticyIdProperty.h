//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Slate/AssetPicker/SArticyObjectTileView.h"
#include "PropertyHandle.h"
#include "Customizations/ArticyEditorCustomizationManager.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "ContentBrowserDelegates.h"
#include "AssetPicker/SArticyObjectAssetPicker.h"

namespace ArticyRefPropertyConstants
{
	const FVector2D ThumbnailSize(64, 64);
	const FMargin ThumbnailPadding(2, 2);
}


/** This combo button is 1:1 the same as the normal SComboButton except that its Tick function does not try to Resize the menu.
 * Instead, it only moves the Window. This fixes a problem where the menu would continuously be resized with 1 pixel difference which causes flickering
 * in some edge cases (zoom level -1 and a menu that wasn't fixed size 300x300 like the default asset picker (300x350 would also flicker)
 */
class SFixedSizeMenuComboButton : public SComboButton
{
protected:
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};
/**
 *  REFERENCE: SPropertyEditorAsset, which is the normal asset selection widget
 */
class ARTICYEDITOR_API SArticyIdProperty : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArticyIdProperty) 
		: _TopLevelClassRestriction(UArticyObject::StaticClass()), _bExactClass(false), _bExactClassEditable(true), _bClassFilterEditable(true)
	{}
		SLATE_ATTRIBUTE(FArticyId, ShownObject)
		SLATE_EVENT(FOnAssetSelected, OnArticyObjectSelected)
		SLATE_ATTRIBUTE(UClass*, TopLevelClassRestriction)
		SLATE_ATTRIBUTE(bool, bExactClass)
		SLATE_ATTRIBUTE(bool, bExactClassEditable)
		SLATE_ATTRIBUTE(bool, bClassFilterEditable)
	SLATE_END_ARGS()
/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	// the articy object this widget currently represents
	TWeakObjectPtr<UArticyObject> CachedArticyObject = nullptr;
	mutable FArticyId CurrentArticyId = FArticyId();
	
	TSharedPtr<SArticyObjectTileView> TileView;
	TSharedPtr<SBox> TileContainer;
	TSharedPtr<SBorder> ThumbnailBorder;
	TSharedPtr<FSlateBrush> ImageBrush;
	TSharedPtr<SWidget> ComboButton;
	TSharedPtr<SHorizontalBox> ExtraButtons;
	
	TAttribute<UClass*> TopLevelClassRestriction;
	TAttribute<bool> bExactClass = false;
	TAttribute<bool> bExactClassEditable = true;
	TAttribute<bool> bClassFilterEditable = true;
private:
	/** Updates the widget including customizations. Called when the selected object changes */
	void UpdateWidget();
	/** Applies a single customization */
	void ApplyArticyRefCustomization(const FArticyRefWidgetCustomizationInfo& Customization);
	/** Applies the given customizations, such as the ExtraButton extensions */
	void ApplyArticyRefCustomizations(const TArray<FArticyRefWidgetCustomizationInfo>& Customizations);

	TSharedRef<SWidget> CreateArticyObjectAssetPicker();
	FReply OnArticyButtonClicked() const;
	FReply OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const;
	FText OnGetArticyObjectDisplayName() const;
	FArticyId GetCurrentObjectID() const;

	//FGeometry ComputeNewWindowMenuPlacement(const FGeometry& AllottedGeometry, const FVector2D& PopupDesiredSize, EMenuPlacement PlacementMode) const;

private:
	/** The current customizations are cached in here to achieve ownership */
	TArray<TSharedPtr<IArticyRefWidgetCustomization>> ActiveCustomizations;

	/** The ExtraButton extenders of the currently active customizations */
	TArray<TSharedPtr<FExtender>> ExtraButtonExtenders;

	TAttribute<FArticyId> ShownObject;
	FOnAssetSelected OnAssetSelected;
};
