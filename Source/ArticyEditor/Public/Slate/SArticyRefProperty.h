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

namespace ArticyRefPropertyConstants {
	const FVector2D ThumbnailSize(64, 64);
	const FMargin ThumbnailPadding(2, 2);

}

/**
 *  REFERENCE: SPropertyEditorAsset, which is the normal asset selection widget
 */
class ARTICYEDITOR_API SArticyRefProperty : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArticyRefProperty) 
		: _ClassRestriction(nullptr), _bExactClass(false)
	{}
		SLATE_ATTRIBUTE(FArticyId, ShownObject)
		SLATE_EVENT(FOnAssetSelected, OnArticyObjectSelected)
		SLATE_ATTRIBUTE(UClass*, ClassRestriction)
		SLATE_ARGUMENT(bool, bExactClass)
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
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SHorizontalBox> ExtraButtons;
	
	TAttribute<UClass*> ClassRestriction;
	bool bExactClass = false;
private:
	/** Updates the widget including customizations. Called when the selected object changes */
	void UpdateWidget();
	/** Applies a single customization */
	void ApplyArticyRefCustomization(const FArticyRefWidgetCustomizationInfo& Customization);
	/** Applies the given customizations, such as the ExtraButton extensions */
	void ApplyArticyRefCustomizations(const TArray<FArticyRefWidgetCustomizationInfo>& Customizations);

	TSharedRef<SWidget> CreateArticyObjectAssetPicker();
	FReply OnArticyButtonClicked() const;
	/** Updates the underlying ArticyRef to reference the new articy object. Can be null */
	void SetAsset(const FAssetData& AssetData) const;
	FReply OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const;
	FText OnGetArticyObjectDisplayName() const;
	FArticyId GetCurrentObjectID() const;

private:
	/** The current customizations are cached in here to achieve ownership */
	TArray<TSharedPtr<IArticyRefWidgetCustomization>> ActiveCustomizations;

	/** The ExtraButton extenders of the currently active customizations */
	TArray<TSharedPtr<FExtender>> ExtraButtonExtenders;

	TAttribute<FArticyId> ShownObject;
	FOnAssetSelected OnAssetSelected;
};
