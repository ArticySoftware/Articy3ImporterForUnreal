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

void SArticyRefProperty::Construct(const FArguments& InArgs, TWeakObjectPtr<UArticyObject> InArticyObject, IPropertyHandle* InArticyRefPropHandle, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	this->ClassRestriction = InArgs._ClassRestriction;

	ArticyObject = InArticyObject;
	ArticyRefPropHandle = InArticyRefPropHandle;
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
		.ObjectToDisplay(ArticyObject)
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
	UserInterfaceHelperFunctions::ShowObjectInArticy(ArticyObject.Get());
	
	return FReply::Handled();
}

void SArticyRefProperty::SetAsset(const FAssetData& AssetData)
{
	// retrieve the newly selected articy object
	ComboButton->SetIsOpen(false);
	ArticyObject = Cast<UArticyObject>(AssetData.GetAsset());

	// construct the new ID
	FArticyId NewId;
	NewId.Low = ArticyObject.IsValid() ? ArticyObject->GetId().Low : 0;
	NewId.High = ArticyObject.IsValid() ? ArticyObject->GetId().High : 0;

	// get the current articy ref struct as formatted string
	FString FormattedValueString;
	ArticyRefPropHandle->GetValueAsFormattedString(FormattedValueString);

	// remove the old ID string
	const int32 IdIndex = FormattedValueString.Find(FString(TEXT("Low=")), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	const int32 EndOfIdIndex = FormattedValueString.Find(FString(TEXT(")")), ESearchCase::IgnoreCase, ESearchDir::FromStart, IdIndex);	
	FormattedValueString.RemoveAt(IdIndex, EndOfIdIndex - IdIndex);

	// reconstruct the value string with the new ID
	const FString NewIdString = FString::Format(TEXT("Low={0}, High={1}"), { NewId.Low, NewId.High, });
	FormattedValueString.InsertAt(IdIndex, *NewIdString);

	// update the actual id of the ArticyObject
	// done via Set functions instead of accessing the ref object directly because using "Set" handles various Unreal logic, such as:
	// - CDO default change forwarding to instances
	// - marking dirty
	// - transaction buffer (Undo, Redo)
	ArticyRefPropHandle->SetValueFromFormattedString(FormattedValueString);	
	
	// Update visuals:
	// recreate the tile view with the newly selected object - can be nullptr
	TileView = SNew(SArticyObjectTileView)
		.ObjectToDisplay(ArticyObject)
		.ThumbnailSize(ArticyRefPropertyConstants::ThumbnailSize.X)
		.ThumbnailPadding(ArticyRefPropertyConstants::ThumbnailPadding.X);

	// activate the new tile view
	TileContainer->SetContent(TileView.ToSharedRef());
}

FReply SArticyRefProperty::OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const
{
	if(ArticyObject.IsValid())
	{
		GEditor->EditObject(ArticyObject.Get());
	}

	return FReply::Handled();
}

FText SArticyRefProperty::OnGetArticyObjectDisplayName() const
{
	const FString DisplayName = UserInterfaceHelperFunctions::GetDisplayName(ArticyObject.Get());
	return FText::FromString(DisplayName);
}

#undef LOCTEXT_NAMESPACE

