// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SDialogueEntityAssetPicker.h"
#include "ArticyAsset.h"
#include "ArticyBuiltinTypes.h"
#include "ArticyObject.h"
#include <EditorStyleSet.h>
#include <ICursor.h>
#include "SDialogueEntityTileView.h"
#include "ArticyGlobalVariables.h"
#include <ContentBrowserModule.h>
#include <TextFilterExpressionEvaluator.h>
#include <WidgetActiveTimerDelegate.h>


#define LOCTEXT_NAMESPACE "DialogueEntityPicker"

SDialogueEntityAssetPicker::~SDialogueEntityAssetPicker()
{

}

void SDialogueEntityAssetPicker::Construct(const FArguments& InArgs)
{
	this->config = InArgs._AssetPickerConfig;

	this->OnAssetSelected = InArgs._AssetPickerConfig.OnAssetSelected;
	Cursor = EMouseCursor::Hand;

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	const bool bCloseSelfOnly = true;

	FrontendFilters = MakeShareable(new FAssetFilterCollectionType());
	FrontendFilters->OnChanged().AddSP(this, &SDialogueEntityAssetPicker::OnFrontendFiltersChanged);
	DialogueEntityFilter = MakeShareable(new FFrontendFilter_DialogueEntity());
	DialogueEntityFilter->SetIncludeAssetPath(false);

	ClassFilter = MakeShareable(new FClassRestrictionFilter());
	// kind of a hack: AssetPickerConfig gets used ONLY to filter with ClassNames[0]. Previous GetAssets with the filter can't be used due to articy objects now being inside of other assets.
	UClass* Class = FindObjectFast<UClass>(nullptr, config.Filter.ClassNames[0], false, true, RF_NoFlags);
	ClassFilter->AllowedClass = Class;
	
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterMenuSelection, nullptr, nullptr, bCloseSelfOnly);

	// add "create asset" button
	/*if (NewAssetFactories.Num() > 0)
	{
		MenuBuilder.BeginSection(NAME_None, LOCTEXT("CreateNewAsset", "Create New Asset"));
		{
			for (UFactory* Factory : NewAssetFactories)
			{
				TWeakObjectPtr<UFactory> FactoryPtr(Factory);

				MenuBuilder.AddMenuEntry(
					Factory->GetDisplayName(),
					Factory->GetToolTip(),
					FSlateIconFinder::FindIconForClass(Factory->GetSupportedClass()),
					FUIAction(FExecuteAction::CreateSP(this, &SPropertyMenuAssetPicker::OnCreateNewAssetSelected, FactoryPtr))
				);
			}
		}
		MenuBuilder.EndSection();
	}*/

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
			LOCTEXT("ClearAsset_ToolTip", "Clears the asset set on this field"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateSP(this, &SDialogueEntityAssetPicker::OnClear))
		);
		
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("BrowseHeader", "Browse"));
	{
		TSharedPtr<SWidget> MenuContent;

		FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

		SAssignNew(SearchBox, SAssetSearchBox)
			.HintText(NSLOCTEXT("ContentBrowser", "SearchBoxHint", "Search Assets"))
			.OnTextChanged(this, &SDialogueEntityAssetPicker::OnSearchBoxChanged)
			.OnTextCommitted(this, &SDialogueEntityAssetPicker::OnSearchBoxCommitted)
			.DelayChangeNotificationsWhileTyping(true);
			//.OnKeyDownHandler(this, &SDialogueEntityAssetPicker::HandleKeyDownFromSearchBox)


		MenuContent =
			SNew(SBox)
			/*.WidthOverride(PropertyEditorAssetConstants::ContentBrowserWindowSize.X)
			.HeightOverride(PropertyEditorAssetConstants::ContentBrowserWindowSize.Y)*/
			.WidthOverride(320)
			.HeightOverride(320)
			[
				SAssignNew(AssetView, STileView<TWeakObjectPtr<UArticyObject>>)
				.SelectionMode(ESelectionMode::Single)
				.ListItemsSource(&FilteredEntities)
				.OnGenerateTile(this, &SDialogueEntityAssetPicker::MakeTileViewWidget)
				.ItemHeight(this, &SDialogueEntityAssetPicker::GetTileViewHeight)
				.ItemWidth(this, &SDialogueEntityAssetPicker::GetTileViewWidth)
				.OnSelectionChanged(this, &SDialogueEntityAssetPicker::UpdateValue)
				.ItemAlignment(EListItemAlignment::EvenlySize)
			];

		MenuBuilder.AddWidget(SearchBox.ToSharedRef(), FText::GetEmpty(), true);
		MenuBuilder.AddWidget(MenuContent.ToSharedRef(), FText::GetEmpty(), true);
	}
	MenuBuilder.EndSection();


	this->ChildSlot
		[
			MenuBuilder.MakeWidget()
		];

	// focus the search bar the next frame
	if (this->config.bFocusSearchBoxWhenOpened)
	{
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SDialogueEntityAssetPicker::SetFocusPostConstruct));
	}

	// filter has be added after the widgets have been created since adding will trigger a callback function that accesses the widgets (FrontEndFilters OnChanged, which causes a refresh)
	FrontendFilters->Add(ClassFilter);

	// refresh is unnecessary due to the new filter addition causing a refresh anyways, but keep it for modularity's sake
	RequestSlowFullListRefresh();
}


void SDialogueEntityAssetPicker::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// taken from assetview.cpp:1189
	if (bSlowFullListRefreshRequested)
	{
		RefreshSourceItems();
		bSlowFullListRefreshRequested = false;
	}
}

TSharedRef<class ITableRow> SDialogueEntityAssetPicker::MakeTileViewWidget(TWeakObjectPtr<UArticyObject> Entity, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedPtr< STableRow<TWeakObjectPtr<UArticyObject>>> TableRowWidget;
	SAssignNew(TableRowWidget, STableRow<TWeakObjectPtr<UArticyObject>>, OwnerTable)
		.Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
		.Cursor(true ? EMouseCursor::GrabHand : EMouseCursor::Default);
	//.OnDragDetected(this, &SAssetView::OnDraggingAssetItem);

	TSharedRef<SDialogueEntityTileView> Item =
		SNew(SDialogueEntityTileView)
		.ObjectToDisplay(Entity)
		.ThumbnailSize(FDialogueEntityAssetPicketConstants::TileSize.X)
		.ThumbnailPadding(FDialogueEntityAssetPicketConstants::ThumbnailPadding);

	TableRowWidget->SetContent(Item);

	return TableRowWidget.ToSharedRef();
}

float SDialogueEntityAssetPicker::GetTileViewHeight() const
{
	return FDialogueEntityAssetPicketConstants::TileSize.Y;
}

float SDialogueEntityAssetPicker::GetTileViewWidth() const
{
	return FDialogueEntityAssetPicketConstants::TileSize.X;
}

void SDialogueEntityAssetPicker::OnClear() const
{
	UpdateValue(nullptr, ESelectInfo::Direct);
}

void SDialogueEntityAssetPicker::RefreshSourceItems()
{
	AllSpeakingEntities.Reset();
	FilteredEntities.Reset();

	// Load the asset registry module
	static const FName AssetRegistryName(TEXT("AssetRegistry"));
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryName);

	// retrieve all assets that pass the FARFilter
	AssetRegistryModule.Get().GetAssets(config.Filter, AllSpeakingEntities);	
	AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetFName(), AllSpeakingEntities);
	
	// then filter by the FrontendFilter
	for(int32 AssetIdx = 0; AssetIdx < AllSpeakingEntities.Num(); AssetIdx++) 
	{
		const FAssetData& AssetData = AllSpeakingEntities[AssetIdx];
		UArticyPackage* ArticyPackage = Cast<UArticyPackage>(AssetData.GetAsset());

		for (const TWeakObjectPtr<UArticyObject> ArticyObject : ArticyPackage->GetAssets())
		{
			FAssetData AssetItem(ArticyObject.Get());
			if (PassesCurrentFrontendFilter(AssetItem))
			{
				if (ArticyObject.IsValid())
				{
					FilteredEntities.Add(ArticyObject);
				}
			}
		}
	}

	AssetView->RequestListRefresh();
}

void SDialogueEntityAssetPicker::SetSearchBoxText(const FText& InSearchText)
{
	// Has anything changed? (need to test case as the operators are case-sensitive)
	if (!InSearchText.ToString().Equals(DialogueEntityFilter->GetRawFilterText().ToString(), ESearchCase::CaseSensitive))
	{
		DialogueEntityFilter->SetRawFilterText(InSearchText);
		if (InSearchText.IsEmpty())
		{
			FrontendFilters->Remove(DialogueEntityFilter);
//			AssetViewPtr->SetUserSearching(false);
		}
		else
		{
			FrontendFilters->Add(DialogueEntityFilter);
//			AssetViewPtr->SetUserSearching(true);
		}
	}
}

void SDialogueEntityAssetPicker::OnFrontendFiltersChanged()
{
	RefreshSourceItems();
}

bool SDialogueEntityAssetPicker::PassesCurrentFrontendFilter(const FAssetData& Item) const
{
	// Check the frontend filters list
	if (FrontendFilters.IsValid() && !FrontendFilters->PassesAllFilters(Item))
	{
		return false;
	}

	return true;
}


EActiveTimerReturnType SDialogueEntityAssetPicker::SetFocusPostConstruct(double InCurrentTime, float InDeltaTime)
{
	if (SearchBox.IsValid())
	{
		FWidgetPath WidgetToFocusPath;
		FSlateApplication::Get().GeneratePathToWidgetUnchecked(SearchBox.ToSharedRef(), WidgetToFocusPath);
		FSlateApplication::Get().SetKeyboardFocus(WidgetToFocusPath, EFocusCause::SetDirectly);
		WidgetToFocusPath.GetWindow()->SetWidgetToFocusOnActivate(SearchBox);

		return EActiveTimerReturnType::Stop;
	}

	return EActiveTimerReturnType::Continue;
}

void SDialogueEntityAssetPicker::RequestSlowFullListRefresh()
{
	bSlowFullListRefreshRequested = true;
}

void SDialogueEntityAssetPicker::UpdateValue(TWeakObjectPtr<UArticyObject> AssetItem, ESelectInfo::Type SelectInfo) const
{
	FAssetData NewAsset(AssetItem.Get());
	OnAssetSelected.ExecuteIfBound(NewAsset);
}

void SDialogueEntityAssetPicker::OnSearchBoxChanged(const FText& InSearchText)
{
	SetSearchBoxText(InSearchText);
}

void SDialogueEntityAssetPicker::OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo)
{
	SetSearchBoxText(InSearchText);
}

#undef LOCTEXT_NAMESPACE

