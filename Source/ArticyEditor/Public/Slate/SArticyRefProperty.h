//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Slate/AssetPicker/SArticyObjectTileView.h"
#include "Customizations/ArticyEditorCustomizationManager.h"
#include "SArticyIdProperty.h"
#include "AssetPicker/SArticyObjectAssetPicker.h"

/**
 *  REFERENCE: SPropertyEditorAsset, which is the normal asset selection widget
 */

DECLARE_DELEGATE_OneParam(FOnArticyRefChanged, const FArticyRef&);

class ARTICYEDITOR_API SArticyRefProperty : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SArticyRefProperty)
		: _ArticyRefToDisplay(FArticyRef())
		, _TopLevelClassRestriction(UArticyObject::StaticClass())
		, _bExactClass(false)
		, _bExactClassEditable(true)
		, _bClassFilterEditable(true)
		, _bIsReadOnly(false)
	{}
		SLATE_ATTRIBUTE(FArticyRef, ArticyRefToDisplay)
		SLATE_EVENT(FOnArticyRefChanged, OnArticyRefChanged)
		SLATE_ATTRIBUTE(UClass*, TopLevelClassRestriction)
		SLATE_ATTRIBUTE(bool, bExactClass)
		SLATE_ATTRIBUTE(bool, bExactClassEditable)
		SLATE_ATTRIBUTE(bool, bClassFilterEditable)
		SLATE_ATTRIBUTE(bool, bIsReadOnly)
	SLATE_END_ARGS()

/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void CreateInternalWidgets();

private:
	TAttribute<FArticyRef> ArticyRefToDisplay;
	FOnArticyRefChanged OnArticyRefChanged;
	TAttribute<UClass*> TopLevelClassRestriction;
	TAttribute<bool> bExactClass;
	TAttribute<bool> bExactClassEditable;
	TAttribute<bool> bClassFilterEditable;
	TAttribute<bool> bIsReadOnly;

	void OnArticyIdChanged(const FArticyId& ArticyId);
	FArticyId GetArticyIdToDisplay() const;
	// the articy object this widget currently represents
	TWeakObjectPtr<UArticyObject> CachedArticyObject = nullptr;
	mutable FArticyRef CachedArticyRef = FArticyRef();
	
	TSharedPtr<SArticyIdProperty> ArticyIdProperty;
	TSharedPtr<FExtender> ArticyIdExtender;
private:
	void CreateAdditionalRefWidgets(FToolBarBuilder& Builder);
	/** Updates the internal values, fires delegates */
	void Update(const FArticyRef& NewRef);
	/** Updates the widget including customizations */
	void UpdateWidget();
private:
	void OnCopyProperty() const;
	void OnPasteProperty();
	bool CanPasteProperty() const;	
};
