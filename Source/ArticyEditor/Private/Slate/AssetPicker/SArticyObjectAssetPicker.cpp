//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/AssetPicker/SArticyObjectAssetPicker.h"
#include "ArticyObject.h"
#include "EditorStyleSet.h"
#include "GenericPlatform/ICursor.h"
#include "Slate/AssetPicker/SArticyObjectTileView.h"
#include "ArticyGlobalVariables.h"
#include <ContentBrowserModule.h>
#include "ArticyPluginSettings.h"
#include "Customizations/Details/ArticyIdCustomization.h"
#include "Types/WidgetActiveTimerDelegate.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SSpacer.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/SlateApplication.h"
#include "ArticyEditorModule.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "ArticyObjectAssetPicker"

SArticyObjectAssetPicker::~SArticyObjectAssetPicker()
{

}

void SArticyObjectAssetPicker::Construct(const FArguments& InArgs)
{
	OnAssetSelected = InArgs._OnArticyObjectSelected;
	OnClassPicked = InArgs._OnClassPicked;
	CurrentClassRestriction = InArgs._CurrentClassRestriction;
	TopLevelClassRestriction = InArgs._TopLevelClassRestriction;
	bExactClass = InArgs._bExactClass;
	bExactClassEditable = InArgs._bExactClassEditable;
	bClassFilterEditable = InArgs._bClassFilterEditable;

	if(!CurrentClassRestriction->IsChildOf(TopLevelClassRestriction.Get()))
	{
		CurrentClassRestriction = TopLevelClassRestriction.Get();
	}
	
	SetCursor(EMouseCursor::Hand);

	const bool bInShouldCloseWindowAfterMenuSelection = true;
	const bool bCloseSelfOnly = true;

	FrontendFilters = MakeShareable(new FArticyObjectFilterCollectionType());
	FrontendFilters->OnChanged().AddSP(this, &SArticyObjectAssetPicker::OnFrontendFiltersChanged);
	ArticyObjectFilter = MakeShareable(new FFrontendFilter_ArticyObject());
	ClassFilter = MakeShareable(new FArticyClassRestrictionFilter(CurrentClassRestriction, bExactClass.Get()));

	CreateInternalWidgets();

	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterMenuSelection, nullptr, nullptr, bCloseSelfOnly);
	MenuBuilder.BeginSection(NAME_None, LOCTEXT("CurrentAssetOperationsHeader", "Current Asset"));
	{		
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
		MenuBuilder.AddWidget(SearchField.ToSharedRef(), FText::GetEmpty(), true);
		MenuBuilder.AddWidget(AssetViewContainer.ToSharedRef(), FText::GetEmpty(), true);
	}
	MenuBuilder.EndSection();

	MenuBuilder.BeginSection(NAME_None, LOCTEXT("FilterHeader", "Filter"));
	{
		MenuBuilder.AddWidget(FilterBox.ToSharedRef(), FText::GetEmpty(), true);
	}
	MenuBuilder.EndSection();
	
	this->ChildSlot
	[
		MenuBuilder.MakeWidget()
	];

	// focus the search field next frame
	RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateSP(this, &SArticyObjectAssetPicker::FocusSearchField));

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

void SArticyObjectAssetPicker::CreateInternalWidgets()
{	
	SAssignNew(SearchField, SAssetSearchBox)
	.HintText(NSLOCTEXT("ContentBrowser", "SearchBoxHint", "Search Assets"))
	.OnTextChanged(this, &SArticyObjectAssetPicker::OnSearchBoxChanged)
	.OnTextCommitted(this, &SArticyObjectAssetPicker::OnSearchBoxCommitted)
	.DelayChangeNotificationsWhileTyping(true);

	SAssignNew(AssetViewContainer, SBox)
	.WidthOverride(325)
	.HeightOverride(325)
	[
		SAssignNew(AssetView, STileView<TWeakObjectPtr<UArticyObject>>)
		.SelectionMode(ESelectionMode::Single)
		.ListItemsSource(&FilteredObjects)
		.OnGenerateTile(this, &SArticyObjectAssetPicker::MakeTileViewWidget)
		.ItemHeight(this, &SArticyObjectAssetPicker::GetTileViewHeight)
		.ItemWidth(this, &SArticyObjectAssetPicker::GetTileViewWidth)
		.OnSelectionChanged(this, &SArticyObjectAssetPicker::SelectAsset)
		.ItemAlignment(EListItemAlignment::EvenlyDistributed)
	];

	ClassFilterButton = SNew(SComboButton)
	.OnGetMenuContent(this, &SArticyObjectAssetPicker::CreateClassPicker)
	.IsEnabled_Lambda([this]() -> bool
	{
		return bClassFilterEditable.Get();
	})
	//.ContentPadding(2.f)
	.ButtonContent()
	[
		SNew(STextBlock)
		.Text(this, &SArticyObjectAssetPicker::GetChosenClassName)
	];
	
	SAssignNew(FilterBox, SHorizontalBox)
	+ SHorizontalBox::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	.AutoWidth()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		.Padding(3.f)
		[
			SNew(STextBlock).Text(FText::FromString("Exact Class "))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		//.Padding(3.f)
		[
			SNew(SCheckBox)
			.IsEnabled(bExactClassEditable)				
			.IsChecked_Lambda([=]()
			{
				return bExactClass.Get() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged(this, &SArticyObjectAssetPicker::OnExactClassCheckBoxChanged)	
		]
	]
	+ SHorizontalBox::Slot()
	.FillWidth(1.f)
	[
		SNew(SSpacer)		
	]
    + SHorizontalBox::Slot()
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Right)
	.AutoWidth()
	.MaxWidth(200.f)
    [
        ClassFilterButton.ToSharedRef()
	];
}

void SArticyObjectAssetPicker::OnCopyProperty(FArticyId Id) const
{
	FString ValueString = Id.ToString();
	FPlatformApplicationMisc::ClipboardCopy(*ValueString);
}

TSharedRef<SWidget> SArticyObjectAssetPicker::CreateClassPicker()
{
	FClassViewerInitializationOptions ClassViewerConfig;
	ClassViewerConfig.DisplayMode = EClassViewerDisplayMode::ListView;
	ClassViewerConfig.bAllowViewOptions = true;
#if ENGINE_MAJOR_VERSION >= 5
	ClassViewerConfig.ClassFilters.Add(MakeShareable(new FArticyRefClassFilter(TopLevelClassRestriction.Get(), false)));
#else
	ClassViewerConfig.ClassFilter = MakeShareable(new FArticyRefClassFilter(TopLevelClassRestriction.Get(), false));
#endif

	return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(ClassViewerConfig, FOnClassPicked::CreateRaw(this, &SArticyObjectAssetPicker::OnClassPicked_Func));
}

void SArticyObjectAssetPicker::OnClassPicked_Func(UClass* InChosenClass)
{
	CurrentClassRestriction = InChosenClass;
	ClassFilter->UpdateFilteredClass(CurrentClassRestriction);
	ClassFilterButton->SetIsOpen(false, false);

	OnClassPicked.ExecuteIfBound(InChosenClass);
}

FText SArticyObjectAssetPicker::GetChosenClassName() const
{
	if (CurrentClassRestriction) 
	{
		return FText::FromString(CurrentClassRestriction->GetName());
	}

	return FText::FromString("None");
}

TSharedRef<class ITableRow> SArticyObjectAssetPicker::MakeTileViewWidget(TWeakObjectPtr<UArticyObject> Entity, const TSharedRef<STableViewBase>& OwnerTable) const
{
	TSharedPtr< STableRow<TWeakObjectPtr<UArticyObject>>> TableRowWidget;
	SAssignNew(TableRowWidget, STableRow<TWeakObjectPtr<UArticyObject>>, OwnerTable)
		.Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
		.Cursor(true ? EMouseCursor::GrabHand : EMouseCursor::Default)
		.Padding(3.f);

	FUIAction CopyAction;
	CopyAction.ExecuteAction = FExecuteAction::CreateSP(this, &SArticyObjectAssetPicker::OnCopyProperty, Entity->GetId());
	
	// create the new tile view; the object to display is fixed so it can't change without the asset picker being recreated.
	TSharedRef<SArticyObjectTileView> Item =
		SNew(SArticyObjectTileView)
		.bIsReadOnly(true)
		.CopyAction(CopyAction)
		.ArticyIdToDisplay(Entity->GetId())
		.ThumbnailSize(FArticyObjectAssetPicketConstants::TileSize)
		.ThumbnailPadding(FArticyObjectAssetPicketConstants::ThumbnailPadding);

	TableRowWidget->SetContent(Item);

	return TableRowWidget.ToSharedRef();
}

float SArticyObjectAssetPicker::GetTileViewHeight() const
{
	return FArticyObjectAssetPicketConstants::TileSize.Y + 2*FArticyObjectAssetPicketConstants::ThumbnailPadding;
}

float SArticyObjectAssetPicker::GetTileViewWidth() const
{
	return FArticyObjectAssetPicketConstants::TileSize.X + 2*FArticyObjectAssetPicketConstants::ThumbnailPadding;
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
	FSlateApplication::Get().DismissAllMenus();
}

void SArticyObjectAssetPicker::OnExactClassCheckBoxChanged(ECheckBoxState NewState)
{
	bExactClass = NewState == ECheckBoxState::Checked;
	ClassFilter->UpdateExactClass(bExactClass.Get());
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

