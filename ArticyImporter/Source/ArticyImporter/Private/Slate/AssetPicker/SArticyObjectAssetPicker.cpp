//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "Slate/AssetPicker/SArticyObjectAssetPicker.h"
#include "ArticyAsset.h"
#include "ArticyBuiltinTypes.h"
#include "ArticyObject.h"
#include "EditorStyleSet.h"
#include "GenericPlatform/ICursor.h"
#include "Slate/AssetPicker/SArticyObjectTileView.h"
#include "ArticyGlobalVariables.h"
#include <ContentBrowserModule.h>
#include "Types/WidgetActiveTimerDelegate.h"


#define LOCTEXT_NAMESPACE "ArticyObjectAssetPicker"

SArticyObjectAssetPicker::~SArticyObjectAssetPicker()
{

}

void SArticyObjectAssetPicker::Construct(const FArguments& InArgs)
{
	AssetPickerConfig = InArgs._AssetPickerConfig;

	OnAssetSelected = InArgs._AssetPickerConfig.OnAssetSelected;
	Cursor = EMouseCursor::Hand;

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	const bool bCloseSelfOnly = true;

	FrontendFilters = MakeShareable(new FAssetFilterCollectionType());
	FrontendFilters->OnChanged().AddSP(this, &SArticyObjectAssetPicker::OnFrontendFiltersChanged);
	ArticyObjectFilter = MakeShareable(new FFrontendFilter_ArticyObject());

	ClassFilter = MakeShareable(new FArticyClassRestrictionFilter());
	// kind of a hack: AssetPickerConfig gets used ONLY to filter with ClassNames[0]
	UClass* Class = FindObjectFast<UClass>(nullptr, AssetPickerConfig.Filter.ClassNames[0], false, true, RF_NoFlags);
	ClassFilter->AllowedClass = Class;
	
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterMenuSelection, nullptr, nullptr, bCloseSelfOnly);

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("CurrentAssetOperationsHeader", "Current Asset"));
	{
		/*if (CurrentObject.IsValid())
		{
			MenuBuilder.AddMenuEntry(
				LOCTEXT("EditAsset", "Edit"),
				LOCTEXT("EditAsset_Tooltip", "Edit this asset"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateSP(this, &SPropertyMenuAssetPicker::OnEdit)));
		}

		if (bAllowCopyPaste)
		{
			MenuBuilder.AddMenuEntry(
				LOCTEXT("CopyAsset", "Copy"),
				LOCTEXT("CopyAsset_Tooltip", "Copies the asset to the clipboard"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateSP(this, &SPropertyMenuAssetPicker::OnCopy))
			);

			MenuBuilder.AddMenuEntry(
				LOCTEXT("PasteAsset", "Paste"),
				LOCTEXT("PasteAsset_Tooltip", "Pastes an asset from the clipboard to this field"),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateSP(this, &SPropertyMenuAssetPicker::OnPaste),
					FCanExecuteAction::CreateSP(this, &SPropertyMenuAssetPicker::CanPaste))
			);
		}*/

		
		MenuBuilder.AddMenuEntry(
			LOCTEXT("ClearAsset", "Clear"),
			LOCTEXT("ClearAsset_ToolTip", "Clears the articy object reference"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateSP(this, &SArticyObjectAssetPicker::OnClear))
		);
		
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("BrowseHeader", "Browse"));
	{
		SAssignNew(SearchField, SAssetSearchBox)
			.HintText(NSLOCTEXT("ContentBrowser", "SearchBoxHint", "Search Assets"))
			.OnTextChanged(this, &SArticyObjectAssetPicker::OnSearchBoxChanged)
			.OnTextCommitted(this, &SArticyObjectAssetPicker::OnSearchBoxCommitted)
			.DelayChangeNotificationsWhileTyping(true);


		const TSharedPtr<SWidget> MenuContent =
			SNew(SBox)
			.WidthOverride(320)
			.HeightOverride(320)
			[
				SAssignNew(AssetView, STileView<TWeakObjectPtr<UArticyObject>>)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(&FilteredObjects)
				.OnGenerateTile(this, &SArticyObjectAssetPicker::MakeTileViewWidget)
				.ItemHeight(this, &SArticyObjectAssetPicker::GetTileViewHeight)
				.ItemWidth(this, &SArticyObjectAssetPicker::GetTileViewWidth)
				.OnSelectionChanged(this, &SArticyObjectAssetPicker::SelectAsset)
				.ItemAlignment(EListItemAlignment::EvenlySize)
			];

		MenuBuilder.AddWidget(SearchField.ToSharedRef(), FText::GetEmpty(), true);
		MenuBuilder.AddWidget(MenuContent.ToSharedRef(), FText::GetEmpty(), true);
	}
	MenuBuilder.EndSection();

	this->ChildSlot
	[
		MenuBuilder.MakeWidget()
	];

	// focus the search bar the next frame
	if (AssetPickerConfig.bFocusSearchBoxWhenOpened)
	{
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SArticyObjectAssetPicker::FocusSearchField));
	}

	// filter has be added after the widgets have been created since adding will trigger a callback function that accesses the widgets (FrontEndFilters OnChanged, which causes a refresh)
	FrontendFilters->Add(ClassFilter);

	// refresh is unnecessary due to the new filter addition causing a refresh anyways, but keep it for clarity's sake
	RequestSlowFullListRefresh();
}


void SArticyObjectAssetPicker::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// reference: assetview.cpp:1189
	if (bSlowFullListRefreshRequested)
	{
		RefreshSourceItems();
		bSlowFullListRefreshRequested = false;
	}
}

TSharedRef<class ITableRow> SArticyObjectAssetPicker::MakeTileViewWidget(TWeakObjectPtr<UArticyObject> Entity, const TSharedRef<STableViewBase>& OwnerTable) const
{
	TSharedPtr< STableRow<TWeakObjectPtr<UArticyObject>>> TableRowWidget;
	SAssignNew(TableRowWidget, STableRow<TWeakObjectPtr<UArticyObject>>, OwnerTable)
		.Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
		.Cursor(true ? EMouseCursor::GrabHand : EMouseCursor::Default);

	TSharedRef<SArticyObjectTileView> Item =
		SNew(SArticyObjectTileView)
		.ObjectToDisplay(Entity)
		.ThumbnailSize(FArticyObjectAssetPicketConstants::TileSize.X)
		.ThumbnailPadding(FArticyObjectAssetPicketConstants::ThumbnailPadding);

	TableRowWidget->SetContent(Item);

	return TableRowWidget.ToSharedRef();
}

float SArticyObjectAssetPicker::GetTileViewHeight() const
{
	return FArticyObjectAssetPicketConstants::TileSize.Y;
}

float SArticyObjectAssetPicker::GetTileViewWidth() const
{
	return FArticyObjectAssetPicketConstants::TileSize.X;
}

void SArticyObjectAssetPicker::OnClear() const
{
	SelectAsset(nullptr, ESelectInfo::Direct);
}

void SArticyObjectAssetPicker::RefreshSourceItems()
{
	ArticyPackageDataAssets.Reset();
	FilteredObjects.Reset();

	// Load the asset registry module
	static const FName AssetRegistryName(TEXT("AssetRegistry"));
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryName);

	// retrieve all articy packages
	AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetFName(), ArticyPackageDataAssets);
	
	// test the contained articy objects against the frontend filters
	for(const FAssetData& ArticyPackageAssetData : ArticyPackageDataAssets)
	{
		UArticyPackage* ArticyPackage = Cast<UArticyPackage>(ArticyPackageAssetData.GetAsset());

		for (const TWeakObjectPtr<UArticyObject> ArticyObject : ArticyPackage->GetAssets())
		{
			FAssetData AssetItem(ArticyObject.Get());
			if (ArticyObject.IsValid() && TestAgainstFrontendFilters(AssetItem))
			{
				FilteredObjects.Add(ArticyObject);
			}
		}
	}

	AssetView->RequestListRefresh();
}

void SArticyObjectAssetPicker::SetSearchBoxText(const FText& InSearchText) const
{
	// Update the filter text only if it has actually changed, including case sensitivity (as operators are case sensitive)
	if (!InSearchText.ToString().Equals(ArticyObjectFilter->GetRawFilterText().ToString(), ESearchCase::CaseSensitive))
	{
		ArticyObjectFilter->SetRawFilterText(InSearchText);

		// add or remove the filter depending on whether the search field has content or not
		if (InSearchText.IsEmpty())
		{
			FrontendFilters->Remove(ArticyObjectFilter);
		}
		else
		{
			FrontendFilters->Add(ArticyObjectFilter);
		}
	}
}

void SArticyObjectAssetPicker::OnFrontendFiltersChanged()
{
	RefreshSourceItems();
}

bool SArticyObjectAssetPicker::TestAgainstFrontendFilters(const FAssetData& Item) const
{
	if (FrontendFilters.IsValid() && !FrontendFilters->PassesAllFilters(Item))
	{
		return false;
	}

	return true;
}


EActiveTimerReturnType SArticyObjectAssetPicker::FocusSearchField(double InCurrentTime, float InDeltaTime) const
{
	if(!SearchField.IsValid())
	{
		return EActiveTimerReturnType::Continue;
	}
	
	FWidgetPath WidgetToFocusPath;
	FSlateApplication::Get().GeneratePathToWidgetUnchecked(SearchField.ToSharedRef(), WidgetToFocusPath);
	FSlateApplication::Get().SetKeyboardFocus(WidgetToFocusPath, EFocusCause::SetDirectly);
	WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate(SearchField);

	return EActiveTimerReturnType::Stop;
}

void SArticyObjectAssetPicker::RequestSlowFullListRefresh()
{
	bSlowFullListRefreshRequested = true;
}

void SArticyObjectAssetPicker::SelectAsset(TWeakObjectPtr<UArticyObject> AssetItem, ESelectInfo::Type SelectInfo) const
{
	const FAssetData NewAsset(AssetItem.Get());
	OnAssetSelected.ExecuteIfBound(NewAsset);
}

void SArticyObjectAssetPicker::OnSearchBoxChanged(const FText& InSearchText) const
{
	SetSearchBoxText(InSearchText);
}

void SArticyObjectAssetPicker::OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo) const
{
	SetSearchBoxText(InSearchText);
}

#undef LOCTEXT_NAMESPACE

