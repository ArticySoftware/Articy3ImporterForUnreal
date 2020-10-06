
//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ContentBrowserDelegates.h"
#include "ArticyObject.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STableViewBase.h>
#include <Widgets/Views/STableRow.h>
#include <IContentBrowserSingleton.h>
#include <SAssetSearchBox.h>
#include <Types/SlateEnums.h>
#include <Misc/TextFilterExpressionEvaluator.h>
#include "Widgets/Input/SComboButton.h"
#include "Slate/ArticyFilterHelpers.h"
#include "ClassViewerModule.h"

#define LOCTEXT_NAMESPACE "ArticyObjectAssetPicker"

namespace FArticyObjectAssetPicketConstants {

	const FVector2D TileSize(96.f, 96.f);
	const int32 ThumbnailPadding = 2;

}

class ARTICYEDITOR_API SArticyObjectAssetPicker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArticyObjectAssetPicker) :
	_CurrentClassRestriction(UArticyObject::StaticClass()),
	_TopLevelClassRestriction(UArticyObject::StaticClass()),
	_bExactClass(false),
	_bExactClassEditable(true),
	_bClassFilterEditable(true)
	{}
		SLATE_EVENT(FOnAssetSelected, OnArticyObjectSelected)
		SLATE_ARGUMENT(UClass*, CurrentClassRestriction)
		SLATE_ATTRIBUTE(UClass*, TopLevelClassRestriction)
		SLATE_ATTRIBUTE(bool, bExactClass)
		SLATE_ATTRIBUTE(bool, bExactClassEditable)
		SLATE_ATTRIBUTE(bool, bClassFilterEditable)
		SLATE_EVENT(FOnClassPicked, OnClassPicked)
	SLATE_END_ARGS()

	~SArticyObjectAssetPicker();

	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void RequestSlowFullListRefresh();
	void SelectAsset(TWeakObjectPtr<UArticyObject> AssetItem, ESelectInfo::Type SelectInfo) const;
private:
	void CreateInternalWidgets();
	void OnCopyProperty(FArticyId Id) const;
	TSharedRef<SWidget> CreateClassPicker();
	void OnExactClassCheckBoxChanged(ECheckBoxState NewState);
	void OnClassPicked_Func(UClass* InChosenClass);
	FText GetChosenClassName() const;
	TSharedRef<class ITableRow> MakeTileViewWidget(TWeakObjectPtr<UArticyObject> Entity, const TSharedRef<STableViewBase>& OwnerTable) const;
	float GetTileViewHeight() const;
	float GetTileViewWidth() const;
	void OnClear() const;
	void OnSearchBoxChanged(const FText& InSearchText) const;
	void OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo) const;
	void RefreshSourceItems();
	void SetSearchBoxText(const FText& InSearchText) const;
	void OnFrontendFiltersChanged();
	bool TestAgainstFrontendFilters(const FAssetData& Item) const;
	/** parameters unused but required by the delegate */
	EActiveTimerReturnType FocusSearchField(double InCurrentTime, float InDeltaTime) const;
	
private: // Slate Attributes
	FOnAssetSelected OnAssetSelected;
	FOnClassPicked OnClassPicked;
	TAttribute<UClass*> TopLevelClassRestriction;
	UClass* CurrentClassRestriction = nullptr;
	TAttribute<bool> bExactClass;
	TAttribute<bool> bExactClassEditable;
	TAttribute<bool> bClassFilterEditable;
	
private:// Widgets
	TSharedPtr<SAssetSearchBox> SearchField;
	TSharedPtr<SWidget> AssetViewContainer;
	TSharedPtr<SListView<TWeakObjectPtr<UArticyObject>>> AssetView;
	TSharedPtr<SHorizontalBox> FilterBox;
	TSharedPtr<SComboButton> ClassFilterButton;
	
private: // Internal Data
	TSharedPtr<FAssetFilterCollectionType> FrontendFilters;
	TSharedPtr<FArticyClassRestrictionFilter> ClassFilter;
	TSharedPtr<FFrontendFilter_ArticyObject> ArticyObjectFilter;
	
	TArray<FAssetData> ArticyPackageDataAssets;
	TArray<TWeakObjectPtr<UArticyObject>> FilteredObjects;
	bool bSlowFullListRefreshRequested = false;
};

#undef LOCTEXT_NAMESPACE