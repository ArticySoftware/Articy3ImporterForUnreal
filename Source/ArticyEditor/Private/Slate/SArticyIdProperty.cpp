//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/SArticyIdProperty.h"
#include <Templates/SharedPointer.h>
#include <Kismet2/KismetEditorUtilities.h>
#include <Kismet2/SClassPickerDialog.h>
#include <ARFilter.h>
#include <IContentBrowserSingleton.h>
#include <ContentBrowserModule.h>
#include <Widgets/Input/SComboButton.h>
#include "ArticyObject.h"
#include "ArticyEditorModule.h"
#include "Slate/AssetPicker/SArticyObjectAssetPicker.h"
#include "Editor.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "Customizations/Details/ArticyIdCustomization.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxDefs.h"
#include "Layout/LayoutUtils.h"

#define LOCTEXT_NAMESPACE "ArticyRefProperty"

void SFixedSizeMenuComboButton::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	TSharedPtr<SWindow> PopupWindow = PopupWindowPtr.Pin();
	if (PopupWindow.IsValid() && IsOpenViaCreatedWindow())
	{
		// Figure out where our attached pop-up window should be placed.
		const FVector2D PopupContentDesiredSize = PopupWindow->GetContent()->GetDesiredSize();
		//const FVector2D PopupContentDesiredSize = PopupWindow->GetContent()->GetDesiredSize();
		FGeometry PopupGeometry = ComputeNewWindowMenuPlacement(AllottedGeometry, PopupContentDesiredSize, Placement.Get());
		const FVector2D NewPosition = PopupGeometry.LocalToAbsolute(FVector2D::ZeroVector);

		// We made a window for showing the popup.
		// Update the window's position!
		PopupWindow->MoveWindowTo(NewPosition);
	}

	/** The tick is ending, so the window was not dismissed this tick. */
	bDismissedThisTick = false;
}

void SArticyIdProperty::Construct(const FArguments& InArgs)
{
	this->ShownObject = InArgs._ShownObject;
	this->OnAssetSelected = InArgs._OnArticyObjectSelected;
	this->TopLevelClassRestriction = InArgs._TopLevelClassRestriction;
	this->bExactClass = InArgs._bExactClass;
	this->bExactClassEditable = InArgs._bExactClassEditable;
	this->bClassFilterEditable = InArgs._bClassFilterEditable;
	
	ensure(ShownObject.IsBound() || ShownObject.IsSet());
	Cursor = EMouseCursor::Hand;

	ComboButton = SNew(SFixedSizeMenuComboButton)
	.OnGetMenuContent(this, &SArticyIdProperty::CreateArticyObjectAssetPicker)
	.ButtonContent()
	[
		SNew(STextBlock)
		.Text(this, &SArticyIdProperty::OnGetArticyObjectDisplayName)
	];

	TileView = SNew(SArticyObjectTileView)
	.ObjectToDisplay(this, &SArticyIdProperty::GetCurrentObjectID)
	.OnMouseDoubleClick(this, &SArticyIdProperty::OnAssetThumbnailDoubleClick)
	.ThumbnailSize(ArticyRefPropertyConstants::ThumbnailSize)
	.ThumbnailPadding(ArticyRefPropertyConstants::ThumbnailPadding);

	ExtraButtons = SNew(SHorizontalBox);

	UpdateWidget();
	
	this->ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0, 0, 2, 0)
		[
			SAssignNew(ThumbnailBorder, SBorder)
			.Padding(5.0f)
			[
				SAssignNew(TileContainer, SBox)
				[
					TileView.ToSharedRef()
				]
			]
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(3, 5, 3, 0)
			[
				ComboButton.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(3, 0, 3, 2)
			[
				ExtraButtons.ToSharedRef()
			]
		]
	];
}

void SArticyIdProperty::UpdateWidget()
{
	for (TSharedPtr<IArticyRefWidgetCustomization>& Customization : ActiveCustomizations)
	{
		Customization->UnregisterArticyRefWidgetCustomization();
	}
	ActiveCustomizations.Reset();
	
	// the actual update. This will be forwarded into the tile view and will cause an update
	CurrentArticyId = ShownObject.Get() ? ShownObject.Get() : FArticyId();
	CachedArticyObject = !CurrentArticyId.IsNull() ? UArticyObject::FindAsset(CurrentArticyId) : nullptr;
	
	FArticyEditorModule::Get().GetCustomizationManager()->CreateArticyRefWidgetCustomizations(CachedArticyObject.Get(), ActiveCustomizations);

	FArticyRefWidgetCustomizationBuilder Builder(CachedArticyObject.Get());
	for(TSharedPtr<IArticyRefWidgetCustomization>& Customization : ActiveCustomizations)
	{
		Customization->RegisterArticyRefWidgetCustomization(Builder);
	}

	// this empties the extra buttons box and refills it based on the registered customizations
	ApplyArticyRefCustomizations(Builder.GetCustomizations());
}

void SArticyIdProperty::ApplyArticyRefCustomization(const FArticyRefWidgetCustomizationInfo& Customization)
{
	if(Customization.ExtraButtonExtender.IsValid())
	{
		ExtraButtonExtenders.Add(Customization.ExtraButtonExtender);
	}
}

void SArticyIdProperty::ApplyArticyRefCustomizations(const TArray<FArticyRefWidgetCustomizationInfo>& Customizations)
{
	ExtraButtonExtenders.Empty();
	ExtraButtons->ClearChildren();

	for(const FArticyRefWidgetCustomizationInfo& Info : Customizations)
	{
		ApplyArticyRefCustomization(Info);
	}

	TSharedPtr<FExtender> ExtraButtonExtender = FExtender::Combine(ExtraButtonExtenders);
	FToolBarBuilder Builder(nullptr, FMultiBoxCustomization(TEXT("ExtraButtons")), ExtraButtonExtender);

	// we need to begin a section so the extenders know where to apply themselves
	Builder.BeginSection(TEXT("Base"));
	Builder.EndSection();

	ExtraButtons->AddSlot()
	[
		Builder.MakeWidget()
	];
}

void SArticyIdProperty::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	const FArticyId CurrentRefId = ShownObject.Get() ? ShownObject.Get(): FArticyId();
	if (CurrentRefId != CurrentArticyId || (!CurrentRefId.IsNull() && !CachedArticyObject.IsValid()))
	{
		UpdateWidget();
	}	
}

TSharedRef<SWidget> SArticyIdProperty::CreateArticyObjectAssetPicker()
{
	TSharedRef<SArticyObjectAssetPicker> AssetPicker = SNew(SArticyObjectAssetPicker)
		.OnArticyObjectSelected(OnAssetSelected)
		.CurrentClassRestriction(CachedArticyObject.IsValid() ? CachedArticyObject->UObject::GetClass() : UArticyObject::StaticClass())
		.TopLevelClassRestriction(TopLevelClassRestriction)
		.bExactClass(bExactClass)
		.bExactClassEditable(bExactClassEditable)
		.bClassFilterEditable(bClassFilterEditable);
	return AssetPicker;
}

FReply SArticyIdProperty::OnArticyButtonClicked() const
{
	UserInterfaceHelperFunctions::ShowObjectInArticy(UArticyObject::FindAsset(CurrentArticyId));
	return FReply::Handled();
}

FReply SArticyIdProperty::OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const
{
	if(CachedArticyObject.IsValid())
	{
		GEditor->EditObject(CachedArticyObject.Get());
	}

	return FReply::Handled();
}

FText SArticyIdProperty::OnGetArticyObjectDisplayName() const
{
	const FString DisplayName = UserInterfaceHelperFunctions::GetDisplayName(CachedArticyObject.Get());
	return FText::FromString(DisplayName);
}

FArticyId SArticyIdProperty::GetCurrentObjectID() const
{
	return CurrentArticyId;
}

//FGeometry SArticyIdProperty::ComputeNewWindowMenuPlacement(const FGeometry& AllottedGeometry,
//	const FVector2D& PopupDesiredSize, EMenuPlacement PlacementMode) const
//{
//	// Compute the popup size, offset, and anchor rect  in local space
//	const SMenuAnchor::FPopupPlacement PopupPlacement(AllottedGeometry, PopupDesiredSize, PlacementMode);
//
//	// already handled
//	const bool bAutoAdjustForDPIScale = false;
//
//	// ask the application to compute the proper desktop offset for the anchor. This requires the offsets to be in desktop space.
//	const FVector2D NewPositionDesktopSpace = FSlateApplication::Get().CalculatePopupWindowPosition(
//		TransformRect(AllottedGeometry.GetAccumulatedLayoutTransform(), PopupPlacement.AnchorLocalSpace),
//		TransformVector(AllottedGeometry.GetAccumulatedLayoutTransform(), PopupPlacement.LocalPopupSize),
//		bAutoAdjustForDPIScale,
//		TransformPoint(AllottedGeometry.GetAccumulatedLayoutTransform(), PopupPlacement.LocalPopupOffset),
//		PopupPlacement.Orientation);
//
//	// transform the desktop offset into local space and use that as the layout transform for the child content.
//	return AllottedGeometry.MakeChild(
//		PopupPlacement.LocalPopupSize,
//		FSlateLayoutTransform(TransformPoint(Inverse(AllottedGeometry.GetAccumulatedLayoutTransform()), NewPositionDesktopSpace)));
//}

#undef LOCTEXT_NAMESPACE

