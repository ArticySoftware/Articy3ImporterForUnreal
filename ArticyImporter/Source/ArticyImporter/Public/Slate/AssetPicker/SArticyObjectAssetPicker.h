//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "CoreMinimal.h"
#include "ContentBrowserDelegates.h"
#include "ArticyObject.h"
#include <Widgets/SCompoundWidget.h>
#include <Widgets/Views/STableViewBase.h>
#include <Widgets/Views/STableRow.h>
#include <IContentBrowserSingleton.h>
#include "Interfaces/ArticyObjectWithPreviewImage.h"
#include <SAssetSearchBox.h>
#include <Types/SlateEnums.h>
#include <Misc/TextFilterExpressionEvaluator.h>
#include "ArticyObjectFilterHelpers.h"

#define LOCTEXT_NAMESPACE "ArticyObjectAssetPicker"

namespace FArticyObjectAssetPicketConstants {

	const FVector2D TileSize(96.f, 96.f);
	const int32 ThumbnailPadding = 2;

}

class SArticyObjectAssetPicker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArticyObjectAssetPicker) {}

	/** A struct containing details about how the asset picker should behave */
	SLATE_ARGUMENT(FAssetPickerConfig, AssetPickerConfig)

	SLATE_END_ARGS()

	~SArticyObjectAssetPicker();

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void RequestSlowFullListRefresh();

	void SelectAsset(TWeakObjectPtr<UArticyObject> AssetItem, ESelectInfo::Type SelectInfo) const;
private:
	
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
private:
	
	FAssetPickerConfig AssetPickerConfig;
	TArray<FAssetData> ArticyPackageDataAssets;
	TArray<TWeakObjectPtr<UArticyObject>> FilteredObjects;
	bool bSlowFullListRefreshRequested = false;
private:

	FOnAssetSelected OnAssetSelected;
private:
	
	TSharedPtr<SAssetSearchBox> SearchField;
	TSharedPtr<FAssetFilterCollectionType> FrontendFilters;
	TSharedPtr<FArticyClassRestrictionFilter> ClassFilter;
	TSharedPtr<FFrontendFilter_ArticyObject> ArticyObjectFilter;
	TSharedPtr<SListView<TWeakObjectPtr<UArticyObject>>> AssetView;
};

#undef LOCTEXT_NAMESPACE