//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "Slate/AssetPicker/SArticyObjectTileView.h"
#include <GenericPlatform/ICursor.h>
#include "ArticyBuiltinTypes.h"
#include <Widgets/Layout/SBorder.h>
#include <Widgets/Text/STextBlock.h>
#include <Styling/SlateTypes.h>
#include <Widgets/Layout/SScaleBox.h>
#include <EditorStyleSet.h>
#include "Slate/AssetPicker/SArticyObjectToolTip.h"
#include "ArticyImporterStyle.h"
#include "Editor.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "Interfaces/ArticyObjectWithColor.h"

#define LOCTEXT_NAMESPACE "ArticyObjectTileView"

void SArticyObjectTileView::Construct(const FArguments& InArgs)
{
	ObjectToDisplay = InArgs._ObjectToDisplay;
	ThumbnailSize = InArgs._ThumbnailSize;
	ThumbnailPadding = InArgs._ThumbnailPadding;

	if(!ObjectToDisplay.IsValid())
	{
		this->ChildSlot
			[
				SAssignNew(WidgetContainerBorder, SBorder)
				.Padding(ThumbnailPadding)
				.BorderImage(FArticyImporterStyle::Get().GetBrush("ArticyImporter.AssetPicker.TileBorder.16"))
				[
					SNew(SScaleBox)
					.Stretch(EStretch::ScaleToFit)
					[
						SNew(SImage)
						.Image(FArticyImporterStyle::Get().GetBrush("ArticyImporter.ArticyApplication.64"))
					]
				]
			];

		return;
	}

	TAttribute<FOptionalSize> WidthScaleAttribute = ThumbnailSize / 3.f;
	TAttribute<FOptionalSize> HeightScaleAttribute = ThumbnailSize / 3.f;

	Cursor = EMouseCursor::Hand;

	// retrieve preview image
	PreviewBrush.ImageSize = FVector2D(ThumbnailSize, ThumbnailSize);
	bHasPreviewImage = UserInterfaceHelperFunctions::RetrievePreviewImage(ObjectToDisplay.Get(), PreviewBrush);

	// if we failed getting a preview image, use the default image instead
	if(!bHasPreviewImage)
	{
		PreviewBrush = *UserInterfaceHelperFunctions::GetArticyTypeImage(ObjectToDisplay.Get(), UserInterfaceHelperFunctions::Large);
		//TypeImage = OnGetTypeImage(UserInterfaceHelperFunctions::Large);
	}
	else
	{
		TypeImage = OnGetTypeImage(UserInterfaceHelperFunctions::Medium);
	}

	// retrieve color for the border
	IArticyObjectWithColor* ObjectWithColor = Cast<IArticyObjectWithColor>(ObjectToDisplay);
	bHasColor = ObjectWithColor ? true : false;
	if(bHasColor)
	{
		ArticyObjectColor = ObjectWithColor->GetColor();
	}
	
	PreviewImage = SNew(SImage)
		.Image(this, &SArticyObjectTileView::OnGetEntityImage);

	EntityNameTextStyle = MakeShareable(new FTextBlockStyle(FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("RichTextBlock.Bold")));

	DisplayNameTextBlock = SNew(STextBlock)
		.Text(this, &SArticyObjectTileView::OnGetEntityName)
		.TextStyle(EntityNameTextStyle.Get())
		.Justification(ETextJustify::Center);

	SetToolTip(SNew(SArticyObjectToolTip).ObjectToDisplay(ObjectToDisplay.Get()));
	
	this->ChildSlot
	[
		SAssignNew(WidgetContainerBorder, SBorder)
		.ToolTip(GetToolTip())
		.BorderBackgroundColor(ArticyObjectColor)
		.BorderImage(FArticyImporterStyle::Get().GetBrush("ArticyImporter.AssetPicker.TileBorder.16"))
		[
			SNew(SBox)
			.WidthOverride(ThumbnailSize)
			.HeightOverride(ThumbnailSize)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.FillHeight(0.8)
					[
						SNew(SBox)
						.WidthOverride(ThumbnailSize)
						.HeightOverride(ThumbnailSize)
						.Padding(ThumbnailPadding)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(SScaleBox)
							.Stretch(EStretch::ScaleToFit)
							[
								PreviewImage.ToSharedRef()
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
							DisplayNameTextBlock.ToSharedRef()
						]
					]
				]
				// top left type image: only visible if there is an actual preview image to indicate the type
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				.Padding(3.f)
				[
					SNew(SBox)
					.WidthOverride(WidthScaleAttribute)
					.HeightOverride(HeightScaleAttribute)
					[
						SNew(SImage)
						.Image(TypeImage)
						.Visibility(this, &SArticyObjectTileView::OnHasPreviewImage)
					]
				]
			]
		]
	];
}


FText SArticyObjectTileView::OnGetEntityName() const
{
	return FText::FromString(UserInterfaceHelperFunctions::GetDisplayName(ObjectToDisplay.Get()));
}

const FSlateBrush* SArticyObjectTileView::OnGetEntityImage() const
{	
	if (PreviewBrush.GetRenderingResource().IsValid())
	{
		return &PreviewBrush;
	}

	PreviewBrush = *FArticyImporterStyle::Get().GetBrush("ArticyImporter.AssetPicker.NoImageAvailable");
	
	return &PreviewBrush;
}

EVisibility SArticyObjectTileView::OnHasPreviewImage() const
{
	return bHasPreviewImage ? EVisibility::Visible : EVisibility::Hidden;
}

const FSlateBrush* SArticyObjectTileView::OnGetTypeImage(UserInterfaceHelperFunctions::EImageSize SizeOverride) const
{
	const FSlateBrush* Brush = UserInterfaceHelperFunctions::GetArticyTypeImage(ObjectToDisplay.Get(), SizeOverride);
	return Brush;
}

#undef LOCTEXT_NAMESPACE

