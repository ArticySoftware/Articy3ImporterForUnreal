//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/SArticyIdProperty.h"
#include <Templates/SharedPointer.h>
#include <Kismet2/KismetEditorUtilities.h>
#include <Kismet2/SClassPickerDialog.h>
#include <IContentBrowserSingleton.h>
#include <ContentBrowserModule.h>
#include <Widgets/Input/SComboButton.h>
#include "ArticyObject.h"
#include "ArticyEditorModule.h"
#include "Slate/AssetPicker/SArticyObjectAssetPicker.h"
#include "Editor.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxDefs.h"
#include "HAL/PlatformApplicationMisc.h"

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
	if(ArticyIdToDisplay.IsBound())
	{
		ensureMsgf(InArgs._OnArticyIdChanged.IsBound(), TEXT("Since the shown object is given externally per event, the handler also needs to handle updates."));
	}

	this->ArticyIdToDisplay = InArgs._ArticyIdToDisplay;
	this->OnArticyIdChanged = InArgs._OnArticyIdChanged;
	this->TopLevelClassRestriction = InArgs._TopLevelClassRestriction;
	this->bExactClass = InArgs._bExactClass;
	this->bExactClassEditable = InArgs._bExactClassEditable;
	this->bClassFilterEditable = InArgs._bClassFilterEditable;
	this->bIsReadOnly = InArgs._bIsReadOnly;
	
	Cursor = EMouseCursor::Hand;
	
	CachedArticyId = ArticyIdToDisplay.Get(FArticyId());
	CachedArticyObject = !CachedArticyId.IsNull() ? UArticyObject::FindAsset(CachedArticyId) : nullptr;
	
	CreateInternalWidgets();

	UpdateWidget();

	this->ChildSlot
	[
		ChildBox.ToSharedRef()
	];
}

void SArticyIdProperty::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	const FArticyId CurrentRefId = ArticyIdToDisplay.Get() ? ArticyIdToDisplay.Get() : FArticyId();
	if (CurrentRefId != CachedArticyId || (!CurrentRefId.IsNull() && !CachedArticyObject.IsValid()))
	{
		Update(CurrentRefId);
	}
}

void SArticyIdProperty::CreateInternalWidgets()
{
	ComboButton = SNew(SFixedSizeMenuComboButton)
		.OnGetMenuContent(this, &SArticyIdProperty::CreateArticyObjectAssetPicker)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &SArticyIdProperty::OnGetArticyObjectDisplayName)
		];

	FUIAction CopyAction;
	FUIAction PasteAction;

	CopyAction.ExecuteAction = FExecuteAction::CreateSP(this, &SArticyIdProperty::OnCopyProperty);
	PasteAction.CanExecuteAction = FCanExecuteAction::CreateSP(this, &SArticyIdProperty::CanPasteProperty);
	PasteAction.ExecuteAction = FExecuteAction::CreateSP(this, &SArticyIdProperty::OnPasteProperty);
	
	TileView = SNew(SArticyObjectTileView)
		.ArticyIdToDisplay(ArticyIdToDisplay)
		.OnArticyIdChanged(OnArticyIdChanged)
		.CopyAction(CopyAction)
		.PasteAction(PasteAction)
		.OnMouseDoubleClick(this, &SArticyIdProperty::OnAssetThumbnailDoubleClick)
		.ThumbnailSize(ArticyRefPropertyConstants::ThumbnailSize)
		.ThumbnailPadding(ArticyRefPropertyConstants::ThumbnailPadding);

	ExtraButtons = SNew(SHorizontalBox);

	ChildBox = SNew(SHorizontalBox)
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
		.Padding(3, 0, 3, 0)
		[
			ExtraButtons.ToSharedRef()
		]
	];
}


void SArticyIdProperty::Update(const FArticyId& NewId)
{
	// the actual update. This will be forwarded into the tile view and will cause an update
	CachedArticyId = NewId;
	CachedArticyObject = !CachedArticyId.IsNull() ? UArticyObject::FindAsset(CachedArticyId) : nullptr;

	UpdateWidget();
}

void SArticyIdProperty::UpdateWidget()
{
	for (TSharedPtr<IArticyRefWidgetCustomization>& Customization : ActiveCustomizations)
	{
		Customization->UnregisterArticyRefWidgetCustomization();
	}
	ActiveCustomizations.Reset();
	
	FArticyEditorModule::Get().GetCustomizationManager()->CreateArticyRefWidgetCustomizations(CachedArticyObject.Get(), ActiveCustomizations);

	FArticyRefWidgetCustomizationBuilder Builder(CachedArticyObject.Get());
	for (TSharedPtr<IArticyRefWidgetCustomization>& Customization : ActiveCustomizations)
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
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
		Builder.MakeWidget()
	];
}

TSharedRef<SWidget> SArticyIdProperty::CreateArticyObjectAssetPicker()
{
	TSharedRef<SArticyObjectAssetPicker> AssetPicker = SNew(SArticyObjectAssetPicker)
		.OnArticyObjectSelected(this, &SArticyIdProperty::OnArticyObjectPicked)
		.CurrentClassRestriction(CachedArticyObject.IsValid() ? CachedArticyObject->UObject::GetClass() : UArticyObject::StaticClass())
		.TopLevelClassRestriction(TopLevelClassRestriction)
		.bExactClass(bExactClass)
		.bExactClassEditable(bExactClassEditable)
		.bClassFilterEditable(bClassFilterEditable);
	return AssetPicker;
}

void SArticyIdProperty::OnArticyObjectPicked(const FAssetData& ArticyObjectData) const
{
	UArticyObject* NewObject = Cast<UArticyObject>(ArticyObjectData.GetAsset());

	if(NewObject)
	{
		OnArticyIdChanged.ExecuteIfBound(NewObject->GetId());
	}
	else
	{
		OnArticyIdChanged.ExecuteIfBound(FArticyId());
	}
}

FReply SArticyIdProperty::OnArticyButtonClicked() const
{
	UserInterfaceHelperFunctions::ShowObjectInArticy(UArticyObject::FindAsset(CachedArticyId));
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
	return CachedArticyId;
}

void SArticyIdProperty::OnCopyProperty() const
{
	FString ValueString = CachedArticyId.ToString();
	FPlatformApplicationMisc::ClipboardCopy(*ValueString);
}

void SArticyIdProperty::OnPasteProperty()
{
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	FArticyId NewId;
	bool bSuccess = NewId.InitFromString(ClipboardContent);
	if (ensureMsgf(bSuccess, TEXT("String was garbage, therefore Id was not properly updated")))
	{
		OnArticyIdChanged.ExecuteIfBound(NewId);
		Update(NewId);
	}
}

bool SArticyIdProperty::CanPasteProperty() const
{
	if (bIsReadOnly.Get())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	if (ClipboardContent.IsEmpty() || !ClipboardContent.Contains("Low=") || !ClipboardContent.Contains("High="))
	{
		return false;
	}

	FArticyId CandidateId;
	if(CandidateId.InitFromString(ClipboardContent))
	{
		UArticyObject* Object = UArticyObject::FindAsset(CandidateId);
		if(!Object)
		{
			return false;
		}

		return Object->UObject::GetClass()->IsChildOf(TopLevelClassRestriction.Get());
	}
	
	return false;
}
#undef LOCTEXT_NAMESPACE
