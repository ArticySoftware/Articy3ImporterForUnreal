// Fill out your copyright notice in the Description page of Project Settings.


#include "SDialogueEntityProperty.h"
#include <SharedPointer.h>
#include <ClassViewerModule.h>
#include <KismetEditorUtilities.h>
#include <SClassPickerDialog.h>
#include <ARFilter.h>
#include <IContentBrowserSingleton.h>
#include <ContentBrowserModule.h>
#include <SComboButton.h>
#include <IPropertyTypeCustomization.h>
#include "ArticyObject.h"
#include "ArticyImporter.h"
#include "SDialogueEntityAssetPicker.h"
#include "ArticyImporterStyle.h"
#include "Editor.h"
#include "SButton.h"
#include "UserInterfaceHelperFunctions.h"

#define LOCTEXT_NAMESPACE "DialogueEntityProperty"


void SArticyRefSelection::Construct(const FArguments& InArgs, FArticyRef* InArticyRef, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	this->ClassRestriction = InArgs._ClassRestriction;

	ArticyRef = InArticyRef;
	SelectedArticyObject = UArticyObject::FindAsset(ArticyRef->GetId());

	Cursor = EMouseCursor::Hand;

	if(!this->ClassRestriction.IsBound()) {
		UE_LOG(LogArticyImporter, Warning, TEXT("Tried constructing Dialogue entity property without valid class restriction. Using ArticyObject instead"));
		this->ClassRestriction = UArticyObject::StaticClass();
	}

	ImageBrush = MakeShareable(new FSlateBrush());
	ImageBrush->SetResourceObject(FArticyImporterStyle::Get().GetBrush("ArticyImporter.AssetPicker.NoImageAvailable")->GetResourceObject());

	ComboButton = SNew(SComboButton)
		.OnGetMenuContent(this, &SArticyRefSelection::CreateEntityAssetPicker)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &SArticyRefSelection::OnGetEntityName)
		];

	EntityImage = SNew(SImage)
		.Image(this, &SArticyRefSelection::OnGetEntityImage);


	const FSlateBrush* ArticySoftwareLogo = FArticyImporterStyle::Get().GetBrush("ArticyImporter.ArticyDraftLogo.16");
	
	ArticyButton = SNew(SButton)
		.OnClicked(this, &SArticyRefSelection::OnArticyButtonClicked)
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
			.OnMouseDoubleClick(this, &SArticyRefSelection::OnAssetThumbnailDoubleClick)
			[
				SNew(SBox)
				.WidthOverride(FDialogueEntityPropertyConstants::ThumbnailSize.X)
				.HeightOverride(FDialogueEntityPropertyConstants::ThumbnailSize.Y)
				[
					EntityImage.ToSharedRef()
				]
			]
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SVerticalBox)
			/*+ SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				SNullWidget::NullWidget
			]*/
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



TSharedRef<SWidget> SArticyRefSelection::CreateEntityAssetPicker()
{	
	FAssetPickerConfig config;
	config.OnAssetSelected = FOnAssetSelected::CreateRaw(this, &SArticyRefSelection::SetAsset);
	config.bFocusSearchBoxWhenOpened = true;
	config.Filter.ClassNames.Add(FName(*ClassRestriction.Get()->GetName()));
	config.bPreloadAssetsForContextMenu = true;
	config.Filter.bRecursiveClasses = true;

	return SNew(SDialogueEntityAssetPicker).AssetPickerConfig(config);
}

FReply SArticyRefSelection::OnArticyButtonClicked()
{
	UserInterfaceHelperFunctions::ShowObjectInArticy(SelectedArticyObject.Get());
	
	return FReply::Handled();
}

void SArticyRefSelection::SetAsset(const FAssetData& AssetData)
{
	ComboButton->SetIsOpen(false);
	UArticyObject* NewSelectedArticyObject = Cast<UArticyObject>(AssetData.GetAsset());
	ArticyRef->SetReference(NewSelectedArticyObject);
	SelectedArticyObject = NewSelectedArticyObject;
}

FReply SArticyRefSelection::OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if(SelectedArticyObject.IsValid()) 
	{
		GEditor->EditObject(SelectedArticyObject.Get());
	}

	return FReply::Handled();
}

FText SArticyRefSelection::OnGetEntityName() const
{
	const FString DisplayName = UserInterfaceHelperFunctions::GetDisplayName(SelectedArticyObject.Get());
	return FText::FromString(DisplayName);
}

void SArticyRefSelection::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// left in as a reminder; used in SPropertyEditorAsset to ensure it doesn't crash
	// As we don't need it right now, the comment and code serves as reminder just in case
	/*if (AssetThumbnail.IsValid() && !GIsSavingPackage && !IsGarbageCollecting())
	{
		FAssetData currentAssetData;
		if (EntityHandle->IsValidHandle())
		{
			EntityHandle->GetValue(currentAssetData);
		}
	}*/
}

const FSlateBrush* SArticyRefSelection::OnGetEntityImage() const
{
	UTexture2D* Texture = UserInterfaceHelperFunctions::GetDisplayImage(SelectedArticyObject.Get());

	if(Texture)
	{
		ImageBrush->SetResourceObject(Texture);
	}
	else
	{
		return FArticyImporterStyle::Get().GetBrush("ArticyImporter.AssetPicker.NoImageAvailable");
	}
	
	return ImageBrush.Get();
}

#undef LOCTEXT_NAMESPACE

