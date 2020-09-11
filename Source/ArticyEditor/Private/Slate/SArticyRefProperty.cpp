//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/SArticyRefProperty.h"
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
#include "Customizations/Details/ArticyRefCustomization.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxDefs.h"

#define LOCTEXT_NAMESPACE "ArticyRefProperty"

void SArticyRefProperty::Construct(const FArguments& InArgs)
{
	this->ShownObject = InArgs._ShownObject;
	this->OnAssetSelected = InArgs._OnArticyObjectSelected;
	this->ClassRestriction = InArgs._ClassRestriction;
	this->bExactClass = InArgs._bExactClass;

	ensure(ShownObject.IsBound() || ShownObject.IsSet());
	Cursor = EMouseCursor::Hand;

	if(!this->ClassRestriction.IsBound()) 
	{
		UE_LOG(LogArticyEditor, Warning, TEXT("Tried constructing articy ref property without valid class restriction. Using ArticyObject instead"));
		this->ClassRestriction = UArticyObject::StaticClass();
	}

	ComboButton = SNew(SComboButton)
	.OnGetMenuContent(this, &SArticyRefProperty::CreateArticyObjectAssetPicker)
	.ButtonContent()
	[
		SNew(STextBlock)
		.Text(this, &SArticyRefProperty::OnGetArticyObjectDisplayName)
	];

	TileView = SNew(SArticyObjectTileView)
	.ObjectToDisplay(this, &SArticyRefProperty::GetCurrentObjectID)
	.OnMouseDoubleClick(this, &SArticyRefProperty::OnAssetThumbnailDoubleClick)
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

void SArticyRefProperty::UpdateWidget()
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

void SArticyRefProperty::ApplyArticyRefCustomization(const FArticyRefWidgetCustomizationInfo& Customization)
{
	if(Customization.ExtraButtonExtender.IsValid())
	{
		ExtraButtonExtenders.Add(Customization.ExtraButtonExtender);
	}
}

void SArticyRefProperty::ApplyArticyRefCustomizations(const TArray<FArticyRefWidgetCustomizationInfo>& Customizations)
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

void SArticyRefProperty::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	const FArticyId CurrentRefId = ShownObject.Get() ? ShownObject.Get(): FArticyId();
	if (CurrentRefId != CurrentArticyId || (!CurrentRefId.IsNull() && !CachedArticyObject.IsValid()))
	{
		UpdateWidget();
	}	
}

TSharedRef<SWidget> SArticyRefProperty::CreateArticyObjectAssetPicker()
{	
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = OnAssetSelected;
	AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
	AssetPickerConfig.Filter.ClassNames.Add(FName(*ClassRestriction.Get()->GetName()));
	return SNew(SArticyObjectAssetPicker).AssetPickerConfig(AssetPickerConfig).bExactClass(bExactClass);
}

FReply SArticyRefProperty::OnArticyButtonClicked() const
{
	UserInterfaceHelperFunctions::ShowObjectInArticy(UArticyObject::FindAsset(CurrentArticyId));
	return FReply::Handled();
}

FReply SArticyRefProperty::OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const
{
	if(CachedArticyObject.IsValid())
	{
		GEditor->EditObject(CachedArticyObject.Get());
	}

	return FReply::Handled();
}

FText SArticyRefProperty::OnGetArticyObjectDisplayName() const
{
	const FString DisplayName = UserInterfaceHelperFunctions::GetDisplayName(CachedArticyObject.Get());
	return FText::FromString(DisplayName);
}

FArticyId SArticyRefProperty::GetCurrentObjectID() const
{
	return CurrentArticyId;
}

#undef LOCTEXT_NAMESPACE

