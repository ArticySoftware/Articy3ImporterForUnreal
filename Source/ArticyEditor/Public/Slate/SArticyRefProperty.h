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
		: _ClassRestriction(nullptr)
	{}
		SLATE_ATTRIBUTE(UClass*, ClassRestriction)
	SLATE_END_ARGS()
/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
	void Construct(const FArguments& InArgs, IPropertyHandle* InArticyRefPropHandle);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	mutable FArticyId CurrentObjectID;
	// the articy object this widget currently represents
	TWeakObjectPtr<UArticyObject> CachedArticyObject = nullptr;
	// the property handle for the articy ref that will be manipulated
	IPropertyHandle* ArticyRefPropertyHandle = nullptr;
	
	TSharedPtr<SArticyObjectTileView> TileView;
	TSharedPtr<SBox> TileContainer;
	TSharedPtr<SBorder> ThumbnailBorder;
	TSharedPtr<FSlateBrush> ImageBrush;
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SHorizontalBox> ExtraButtons;
	TAttribute<UClass*> ClassRestriction;
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
};
