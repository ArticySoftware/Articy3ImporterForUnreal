//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
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
#include "ArticyEditorStyle.h"
#include "Editor.h"
#include "Slate/UserInterfaceHelperFunctions.h"

#define LOCTEXT_NAMESPACE "ArticyObjectTileView"

void SArticyObjectTileView::UpdateDisplayedArticyObject()
{
	CachedArticyId = ArticyIdAttribute.Get();
	CachedArticyObject = UArticyObject::FindAsset(CachedArticyId);

	bHasPreviewImage = UserInterfaceHelperFunctions::RetrievePreviewImage(CachedArticyObject.Get(), PreviewBrush);
	// if we failed getting a preview image, use the default type image instead
	if (!bHasPreviewImage)
	{
		PreviewBrush = *UserInterfaceHelperFunctions::GetArticyTypeImage(CachedArticyObject.Get(), UserInterfaceHelperFunctions::Large);
	}

	TypeImage = UserInterfaceHelperFunctions::GetArticyTypeImage(CachedArticyObject.Get(), UserInterfaceHelperFunctions::Medium);

}

void SArticyObjectTileView::Construct(const FArguments& InArgs)
{
	ArticyIdAttribute = InArgs._ObjectToDisplay;
	ThumbnailSize = InArgs._ThumbnailSize;
	ThumbnailPadding = InArgs._ThumbnailPadding;

	TAttribute<FOptionalSize> WidthScaleAttribute = ThumbnailSize / 3.f;
	TAttribute<FOptionalSize> HeightScaleAttribute = ThumbnailSize / 3.f;

	Cursor = EMouseCursor::Hand;

	PreviewBrush.ImageSize = FVector2D(ThumbnailSize, ThumbnailSize);
	
	PreviewImage = SNew(SImage)
		.Image(this, &SArticyObjectTileView::OnGetEntityImage);

	EntityNameTextStyle = MakeShareable(new FTextBlockStyle(FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("RichTextBlock.Bold")));

	DisplayNameTextBlock = SNew(STextBlock)
		.Text(this, &SArticyObjectTileView::OnGetEntityName)
		.TextStyle(EntityNameTextStyle.Get())
		.Justification(ETextJustify::Center);

	UpdateDisplayedArticyObject();

	SetToolTip(SNew(SArticyObjectToolTip).ObjectToDisplay(ArticyIdAttribute));

	this->ChildSlot
	[
		SAssignNew(WidgetContainerBorder, SBorder)
		.ToolTip(GetToolTip())
		.BorderBackgroundColor(this, &SArticyObjectTileView::OnGetArticyObjectColor)
		.BorderImage(FArticyEditorStyle::Get().GetBrush("ArticyImporter.AssetPicker.TileBorder.16"))
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
						.Image(this, &SArticyObjectTileView::GetTypeImage)
						.Visibility(this, &SArticyObjectTileView::OnHasPreviewImage)
					]
				]
			]
		]
	];
}

void SArticyObjectTileView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// if the Id is different from the cached Id, update the widget
	if(CachedArticyId != ArticyIdAttribute.Get() || !CachedArticyObject.IsValid())
	{
		UpdateDisplayedArticyObject();
	}
}


FText SArticyObjectTileView::OnGetEntityName() const
{
	return FText::FromString(UserInterfaceHelperFunctions::GetDisplayName(CachedArticyObject.Get()));
}

const FSlateBrush* SArticyObjectTileView::OnGetEntityImage() const
{	
	if (PreviewBrush.GetRenderingResource().IsValid())
	{
		return &PreviewBrush;
	}
	
	return &PreviewBrush;
}

EVisibility SArticyObjectTileView::OnHasPreviewImage() const
{
	return bHasPreviewImage ? EVisibility::Visible : EVisibility::Hidden;
}

FSlateColor SArticyObjectTileView::OnGetArticyObjectColor() const
{
	return UserInterfaceHelperFunctions::GetColor(CachedArticyObject.Get());
}

const FSlateBrush* SArticyObjectTileView::GetTypeImage() const
{
	return TypeImage;
}

#undef LOCTEXT_NAMESPACE

