//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Slate/AssetPicker/SArticyObjectTileView.h"
#include "IPropertyTypeCustomization.h"

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
	void Construct(const FArguments& InArgs, TWeakObjectPtr<UArticyObject> InArticyObject, IPropertyHandle* InArticyRefPropHandle);

private:
	// the articy object this widget currently represents
	TWeakObjectPtr<UArticyObject> ArticyObject = nullptr;
	// the property handle for the articy ref that will be manipulated
	IPropertyHandle* ArticyRefPropHandle = nullptr;
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
	/** Updates the underlying ArticyRef to reference the new asset. Can be null */
	void SetAsset(const FAssetData& AssetData);
	FReply OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const;
	FText OnGetArticyObjectDisplayName() const;
	
};
