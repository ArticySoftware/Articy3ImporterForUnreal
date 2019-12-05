// Fill out your copyright notice in the Description page of Project Settings.


#include "SDialogueEntityTileView.h"
#include "ArticyFunctionLibrary.h"
#include "ArticyBaseTypes.h"
#include <Engine/Texture2D.h>
#include "ArticyObject.h"
#include <ICursor.h>
#include "ArticyAsset.h"
#include "ArticyBuiltinTypes.h"
#include "ArticyObjectWithPreviewImage.h"
#include <SBorder.h>
#include <STextBlock.h>
#include <SlateTypes.h>
#include <SScaleBox.h>
#include <EditorStyleSet.h>
#include "SDialogueEntityToolTip.h"
#include "ArticyObjectWithText.h"
#include "ArticyObjectWithDisplayName.h"
#include <Kismet/KismetStringLibrary.h>
#include "ArticyObjectWithSpeaker.h"
#include "ArticyImporterStyle.h"
#include "Editor.h"
#include "UserInterfaceHelperFunctions.h"

#define LOCTEXT_NAMESPACE "DialogueEntityTileView"

void SDialogueEntityTileView::Construct(const FArguments& InArgs)
{
	ObjectToDisplay = InArgs._ObjectToDisplay;
	ThumbnailSize = InArgs._ThumbnailSize;
	ThumbnailPadding = InArgs._ThumbnailPadding;

	Cursor = EMouseCursor::Hand;

	ArticyObjectImageBrush = MakeShareable(new FSlateBrush());
	ArticyObjectImageBrush->SetResourceObject(FArticyImporterStyle::Get().GetBrush("ArticyImporter.AssetPicker.NoImageAvailable")->GetResourceObject());
	ArticyObjectImageBrush->ImageSize = FVector2D(ThumbnailSize, ThumbnailSize);

	EntityImage = SNew(SImage)
		.Image(this, &SDialogueEntityTileView::OnGetEntityImage);

	EntityNameTextStyle = MakeShareable(new FTextBlockStyle(FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("RichTextBlock.Bold")));

	EntityNameTextBlock = SNew(STextBlock)
		.Text(this, &SDialogueEntityTileView::OnGetEntityName)
		.TextStyle(EntityNameTextStyle.Get())
		.Justification(ETextJustify::Center);

	SetToolTip(SNew(SDialogueEntityToolTip).ObjectToDisplay(ObjectToDisplay.Get()));

	this->ChildSlot
	[
		SAssignNew(WidgetContainerBorder, SBorder)
		.ToolTip(GetToolTip())
		[
			// text as overlay on image (not well readable though)
			/*SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(ThumbnailSize + ThumbnailPadding)
				.HeightOverride(ThumbnailSize + ThumbnailPadding)
				.Padding(ThumbnailPadding)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					EntityImage.ToSharedRef()
				]
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.Padding(3, 0, 3, 5)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFitX)
				
				[
					EntityNameTextBlock.ToSharedRef()
				]
			]*/

			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.FillHeight(0.8)
			[
				SNew(SBox)
				.WidthOverride(ThumbnailSize + ThumbnailPadding)
				.HeightOverride(ThumbnailSize + ThumbnailPadding)
				.Padding(ThumbnailPadding)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SScaleBox)
					.Stretch(EStretch::ScaleToFit)
					[
						EntityImage.ToSharedRef()
					]
				]
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			.AutoHeight()
			.Padding(2, 0, 2, 3)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::ScaleToFit)
				[
					EntityNameTextBlock.ToSharedRef()
				]
			]
		]
	];
}


FText SDialogueEntityTileView::OnGetEntityName() const
{
	return FText::FromString(UserInterfaceHelperFunctions::GetDisplayName(ObjectToDisplay.Get()));
}

const FSlateBrush* SDialogueEntityTileView::OnGetEntityImage() const
{
	if(CachedTexture.IsValid() && ArticyObjectImageBrush->GetResourceObject() == CachedTexture.Get())
	{
		return ArticyObjectImageBrush.Get();
	}
		
	CachedTexture = UserInterfaceHelperFunctions::GetDisplayImage(ObjectToDisplay.Get());

	if (CachedTexture.IsValid())
	{
		ArticyObjectImageBrush->SetResourceObject(CachedTexture.Get());
	}
	else
	{
		return FArticyImporterStyle::Get().GetBrush("ArticyImporter.AssetPicker.NoImageAvailable");
	}

	return ArticyObjectImageBrush.Get();	
}

#undef LOCTEXT_NAMESPACE

