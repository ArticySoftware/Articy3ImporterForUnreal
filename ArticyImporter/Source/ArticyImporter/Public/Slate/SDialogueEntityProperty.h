// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArticyRef.h"
#include <PropertyHandle.h>
#include "SComboButton.h"
#include "SImage.h"
#include "ArticyBaseTypes.h"
#include "SBorder.h"
#include "Private/SClassViewer.h"

namespace FDialogueEntityPropertyConstants {
	const FVector2D ThumbnailSize(64, 64);

}
/**
 *  REFERENCE: SPropertyEditorAsset, which is the normal asset selection widget
 */
class ARTICYIMPORTER_API SArticyRefSelection : public SCompoundWidget
{
public:	
	SLATE_BEGIN_ARGS(SArticyRefSelection) 
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
	TSharedPtr<SBorder> ThumbnailBorder;
	TSharedPtr<SImage> EntityImage;
	TSharedPtr<FSlateBrush> ImageBrush;
	TSharedPtr<SComboButton> ComboButton;
	TAttribute<UClass*> ClassRestriction;
private:
	TSharedRef<SWidget> CreateEntityAssetPicker();
	void SetAsset(const FAssetData& AssetData);
	FReply OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent);
	FText OnGetEntityName() const;
	const FSlateBrush* OnGetEntityImage() const;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

};
