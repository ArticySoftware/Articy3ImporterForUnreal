//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/AssetPicker/SArticyObjectTileView.h"
#include <GenericPlatform/ICursor.h>
#include <Widgets/Layout/SBorder.h>
#include <Widgets/Text/STextBlock.h>
#include <Styling/SlateTypes.h>
#include <Widgets/Layout/SScaleBox.h>
#include <EditorStyleSet.h>
#include "Slate/AssetPicker/SArticyObjectToolTip.h"
#include "ArticyEditorStyle.h"
#include "Editor.h"
#include "ArticyEditorModule.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "ArticyObjectTileView"

void SArticyObjectTileView::Update(const FArticyId& NewArticyId)
{	
	CachedArticyId = NewArticyId;
	CachedArticyObject = UArticyObject::FindAsset(CachedArticyId);
	
	UpdateWidget();
}

void SArticyObjectTileView::UpdateWidget()
{
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
	ArticyIdToDisplay = InArgs._ArticyIdToDisplay;
	OnArticyIdChanged = InArgs._OnArticyIdChanged;
	ThumbnailSize = InArgs._ThumbnailSize;
	ThumbnailPadding = InArgs._ThumbnailPadding;
	LabelVisibility = InArgs._LabelVisibility;
	bIsReadOnly = InArgs._bIsReadOnly;
	CopyAction = InArgs._CopyAction;
	PasteAction = InArgs._PasteAction;
	
	TAttribute<FOptionalSize> WidthScaleAttribute = ThumbnailSize.X / 3.f;
	TAttribute<FOptionalSize> HeightScaleAttribute = ThumbnailSize.Y / 3.f;

	SetCursor(EMouseCursor::Hand);

	PreviewBrush.ImageSize = ThumbnailSize;
	
	PreviewImage = SNew(SImage)
		.Image(this, &SArticyObjectTileView::OnGetEntityImage);

	EntityNameTextStyle = MakeShareable(new FTextBlockStyle(FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("RichTextBlock.Bold")));

	DisplayNameTextBlock = SNew(STextBlock)
		.Text(this, &SArticyObjectTileView::OnGetEntityName)
		.TextStyle(EntityNameTextStyle.Get())
		.Justification(ETextJustify::Center);

	Update(ArticyIdToDisplay.Get(FArticyId()));

	SetToolTip(SNew(SArticyObjectToolTip).ObjectToDisplay(ArticyIdToDisplay));

	this->SetOnMouseDoubleClick(InArgs._OnMouseDoubleClick);
	
	this->ChildSlot
	[
		SAssignNew(WidgetContainerBorder, SBorder)
		.ToolTip(GetToolTip())
		.BorderBackgroundColor(this, &SArticyObjectTileView::OnGetArticyObjectColor)
		.BorderImage(FArticyEditorStyle::Get().GetBrush("ArticyImporter.AssetPicker.TileBorder.16"))
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(ThumbnailSize.X)
			.HeightOverride(ThumbnailSize.Y)
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
						.WidthOverride(ThumbnailSize.X)
						.HeightOverride(ThumbnailSize.Y)
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
					//.Padding(2, 0, 2, 3)
					[
						SNew(SScaleBox)
						.Stretch(EStretch::ScaleToFit)
						.Visibility(LabelVisibility)
						[
							DisplayNameTextBlock.ToSharedRef()
						]
					]
				]
				// top left type image: only visible if there is an actual preview image to indicate the type
				+ SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				.Padding(0.f, 3.f)
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
	if(CachedArticyId != ArticyIdToDisplay.Get() || (!CachedArticyObject.IsValid() && !CachedArticyId.IsNull()))
	{
		Update(ArticyIdToDisplay.Get());
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

void SArticyObjectTileView::OnContextMenuOpening(FMenuBuilder& Builder) const
{
	// Hide separator line if it only contains the SearchWidget, making the next 2 elements the top of the list
	if (Builder.GetMultiBox()->GetBlocks().Num() > 1)
	{
		Builder.AddMenuSeparator();
	}
	
	if (CopyAction.IsBound())
	{
		Builder.AddMenuEntry(
			NSLOCTEXT("PropertyView", "CopyProperty", "Copy"),
			NSLOCTEXT("PropertyView", "CopyProperty_ToolTip", "Copy this property value"),
			FSlateIcon(FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Copy"),
			CopyAction);

	}
	if(PasteAction.IsBound() && !bIsReadOnly.Get())
	{
		Builder.AddMenuEntry(
			NSLOCTEXT("PropertyView", "PasteProperty", "Paste"),
			NSLOCTEXT("PropertyView", "PasteProperty_ToolTip", "Paste the copied value here"),
			FSlateIcon(FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Paste"),
			PasteAction);
	}
}

FReply SArticyObjectTileView::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FReply Reply = FReply::Handled();
		// this will register this widget as prioritised widget for the mouse button up event, even if child widgets would handle the button up event
		Reply.CaptureMouse(SharedThis(this));
		return Reply;
	}

	return FReply::Unhandled();
}

FReply SArticyObjectTileView::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FMenuBuilder MenuBuilder(true, nullptr, nullptr, true);

		OnContextMenuOpening(MenuBuilder);
		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuBuilder.MakeWidget(), MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);

		return FReply::Handled();
	}

	return FReply::Unhandled();
}
#undef LOCTEXT_NAMESPACE

