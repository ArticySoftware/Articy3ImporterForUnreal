//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "Slate/SArticyRefProperty.h"
#include <Templates/SharedPointer.h>
#include <Kismet2/KismetEditorUtilities.h>
#include <Kismet2/SClassPickerDialog.h>
#include <ARFilter.h>
#include <IContentBrowserSingleton.h>
#include <ContentBrowserModule.h>
#include <Widgets/Input/SComboButton.h>
#include <IPropertyTypeCustomization.h>
#include "ArticyObject.h"
#include "ArticyImporter.h"
#include "Slate/AssetPicker/SArticyObjectAssetPicker.h"
#include "ArticyImporterStyle.h"
#include "Editor.h"
#include "Widgets/Input/SButton.h"
#include "Slate/UserInterfaceHelperFunctions.h"

#define LOCTEXT_NAMESPACE "ArticyRefProperty"


void SArticyRefProperty::Construct(const FArguments& InArgs, FArticyRef* InArticyRef, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	this->ClassRestriction = InArgs._ClassRestriction;

	ArticyRef = InArticyRef;
	SelectedArticyObject = UArticyObject::FindAsset(ArticyRef->GetId());

	Cursor = EMouseCursor::Hand;

	if(!this->ClassRestriction.IsBound()) {
		UE_LOG(LogArticyImporter, Warning, TEXT("Tried constructing Dialogue entity property without valid class restriction. Using ArticyObject instead"));
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
		.ObjectToDisplay(SelectedArticyObject)
		.ThumbnailSize(ArticyRefPropertyConstants::ThumbnailSize.X)
		.ThumbnailPadding(ArticyRefPropertyConstants::ThumbnailPadding.X);
	
	const FSlateBrush* ArticySoftwareLogo = FArticyImporterStyle::Get().GetBrush("ArticyImporter.ArticyDraftLogo.16");
	
	ArticyButton = SNew(SButton)
		.OnClicked(this, &SArticyRefProperty::OnArticyButtonClicked)
		.ToolTipText(FText::FromString("Show selected object in articy:draft"))
		.Content()
		[
			SNew(SImage)
			.Image(ArticySoftwareLogo)
		];
	
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
			//.BorderImage(this, &SPropertyEditorAsset::GetThumbnailBorder)
			.OnMouseDoubleClick(this, &SArticyRefProperty::OnAssetThumbnailDoubleClick)
			[
				SAssignNew(TileContainer, SBox)
			//	.WidthOverride(ArticyRefPropertyConstants::ThumbnailSize.X)
			//	.HeightOverride(ArticyRefPropertyConstants::ThumbnailSize.Y)
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
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
#if PLATFORM_WINDOWS
					ArticyButton.ToSharedRef()
#else
					SNullWidget::NullWidget
#endif
				]
			]
		]
	];
}

TSharedRef<SWidget> SArticyRefProperty::CreateArticyObjectAssetPicker()
{	
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateRaw(this, &SArticyRefProperty::SetAsset);
	AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
	AssetPickerConfig.Filter.ClassNames.Add(FName(*ClassRestriction.Get()->GetName()));

	return SNew(SArticyObjectAssetPicker).AssetPickerConfig(AssetPickerConfig);
}

FReply SArticyRefProperty::OnArticyButtonClicked() const
{
	UserInterfaceHelperFunctions::ShowObjectInArticy(SelectedArticyObject.Get());
	
	return FReply::Handled();
}

void SArticyRefProperty::SetAsset(const FAssetData& AssetData)
{
	ComboButton->SetIsOpen(false);
	UArticyObject* NewSelectedArticyObject = Cast<UArticyObject>(AssetData.GetAsset());
	ArticyRef->SetReference(NewSelectedArticyObject);

	SelectedArticyObject = NewSelectedArticyObject;

	TileView = SNew(SArticyObjectTileView)
		.ObjectToDisplay(SelectedArticyObject)
		.ThumbnailSize(ArticyRefPropertyConstants::ThumbnailSize.X)
		.ThumbnailPadding(ArticyRefPropertyConstants::ThumbnailPadding.X);

	TileContainer->SetContent(TileView.ToSharedRef());
}

FReply SArticyRefProperty::OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const
{
	if(SelectedArticyObject.IsValid()) 
	{
		GEditor->EditObject(SelectedArticyObject.Get());
	}

	return FReply::Handled();
}

FText SArticyRefProperty::OnGetArticyObjectDisplayName() const
{
	const FString DisplayName = UserInterfaceHelperFunctions::GetDisplayName(SelectedArticyObject.Get());
	return FText::FromString(DisplayName);
}

#undef LOCTEXT_NAMESPACE

