// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArticyRef.h"
#include <PropertyHandle.h>
#include "SComboButton.h"
#include "SBorder.h"
#include "Private/SClassViewer.h"
#include "SArticyObjectTileView.h"

namespace ArticyRefPropertyConstants {
	const FVector2D ThumbnailSize(64, 64);
	const FVector2D ThumbnailPadding(2, 2);

}
/**
 *  REFERENCE: SPropertyEditorAsset, which is the normal asset selection widget
 */
class ARTICYIMPORTER_API SArticyRefProperty : public SCompoundWidget
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
	void Construct(const FArguments& InArgs, FArticyRef* InArticyRef, IPropertyTypeCustomizationUtils& CustomizationUtils);

private:
	FArticyRef* ArticyRef = nullptr;
	TWeakObjectPtr<UArticyObject> SelectedArticyObject;
	TSharedPtr<SArticyObjectTileView> TileView;
	TSharedPtr<SBox> TileContainer;
	TSharedPtr<SBorder> ThumbnailBorder;
	TSharedPtr<FSlateBrush> ImageBrush;
	TSharedPtr<SComboButton> ComboButton;
	TSharedPtr<SButton> ArticyButton;
	TAttribute<UClass*> ClassRestriction;
private:
	TSharedRef<SWidget> CreateArticyObjectAssetPicker();
	FReply OnArticyButtonClicked() const;
	void SetAsset(const FAssetData& AssetData);
	FReply OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const;
	FText OnGetArticyObjectDisplayName() const;
	
};
