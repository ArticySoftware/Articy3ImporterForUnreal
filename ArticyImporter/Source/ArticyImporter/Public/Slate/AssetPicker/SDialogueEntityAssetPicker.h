// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <STileView.h>
#include "ContentBrowserDelegates.h"
#include "ArticyObject.h"
#include <SCompoundWidget.h>
#include <STableViewBase.h>
#include <SlateBrush.h>
#include <STableRow.h>
#include <IContentBrowserSingleton.h>
#include "ArticyObjectWithPreviewImage.h"
#include <SAssetSearchBox.h>
#include <SlateEnums.h>
#include <FrontendFilterBase.h>
#include <CollectionManagerTypes.h>
#include <TextFilterExpressionEvaluator.h>
#include <IDelegateInstance.h>
#include <Private/Application/ActiveTimerHandle.h>
#include "ObjectSearchBoxHelpers.h"

#define LOCTEXT_NAMESPACE "DialogueEntityPicker"

namespace FDialogueEntityAssetPicketConstants {

	const FVector2D TileSize(96.f, 96.f);
	const int32 ThumbnailPadding = 2;

}

class SDialogueEntityAssetPicker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDialogueEntityAssetPicker) {}

	/** A struct containing details about how the asset picker should behave */
	SLATE_ARGUMENT(FAssetPickerConfig, AssetPickerConfig)

	SLATE_END_ARGS()

	~SDialogueEntityAssetPicker();

	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void RequestSlowFullListRefresh();

	void UpdateValue(TWeakObjectPtr<UArticyObject> AssetItem, ESelectInfo::Type SelectInfo);
private:
	TSharedRef<class ITableRow> MakeTileViewWidget(TWeakObjectPtr<UArticyObject> Entity, const TSharedRef<STableViewBase>& OwnerTable);
	float GetTileViewHeight() const;
	float GetTileViewWidth() const;
	void OnSearchBoxChanged(const FText& InSearchText);
	void OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo);
	void RefreshSourceItems();
	void SetSearchBoxText(const FText& InSearchText);
	void OnFrontendFiltersChanged();
	bool PassesCurrentFrontendFilter(const FAssetData& Item) const;
	EActiveTimerReturnType SetFocusPostConstruct(double InCurrentTime, float InDeltaTime);
private:
	FAssetPickerConfig config;
	TArray<FAssetData> AllSpeakingEntities;
	TArray<TWeakObjectPtr<UArticyObject>> FilteredEntities;
	bool bSlowFullListRefreshRequested;
private:

	FOnAssetSelected OnAssetSelected;

private:
	// #TODO
	TSharedPtr<SAssetSearchBox> SearchBox;
	TSharedPtr<FAssetFilterCollectionType> FrontendFilters;
	TSharedPtr<FClassRestrictionFilter> ClassFilter;
	TSharedPtr<FFrontendFilter_DialogueEntity> DialogueEntityFilter;
	TSharedPtr<SListView<TWeakObjectPtr<UArticyObject>>> AssetView;
};

#undef LOCTEXT_NAMESPACE