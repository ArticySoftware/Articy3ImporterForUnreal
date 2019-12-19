// Fill out your copyright notice in the Description page of Project Settings.

#include "SArticyObjectToolTip.h"
#include "ArticyFlowClasses.h"
#include "ArticyObjectWithDisplayName.h"
#include "ArticyObjectWithText.h"
#include <SBoxPanel.h>
#include <STextBlock.h>
#include <EditorStyleSet.h>
#include <SBorder.h>
#include <SToolTip.h>
#include <Text.h>
#include "UserInterfaceHelperFunctions.h"
#include "ArticyObjectWithSpeaker.h"
#include "SScaleBox.h"

#define LOCTEXT_NAMESPACE "ArticyObjectToolTip"

void SArticyObjectToolTip::Construct(const FArguments& InArgs)
{
	ObjectToDisplay = InArgs._ObjectToDisplay;

	TooltipBrush.ImageSize = FVector2D(64.f, 64.f);
	// use the preview image if available
	const bool bHasPreviewImage = UserInterfaceHelperFunctions::RetrievePreviewImage(ObjectToDisplay.Get(), TooltipBrush);

	// if there is no preview image, use the preview image of the speaker, if available
	if(!bHasPreviewImage)
	{
		const bool bHasSpeakerPreviewImage = UserInterfaceHelperFunctions::RetrieveSpeakerPreviewImage(ObjectToDisplay.Get(), TooltipBrush);

		// if there is no speaker preview image, use the type image instead
		if(!bHasSpeakerPreviewImage)
		{
			TooltipBrush = *UserInterfaceHelperFunctions::GetArticyTypeImage(ObjectToDisplay.Get(), UserInterfaceHelperFunctions::Large);
		}
	}
	
	SToolTip::Construct(
		SToolTip::FArguments()
		.TextMargin(1.f)
		.BorderImage(FEditorStyle::Get().GetBrush("ContentBrowser.TileViewTooltip.ToolTipBorder"))
		// Text makes tooltip show, probably because it doesn't initialize otherwise
		.Text(FText::FromString("TEST"))
		);

}

void SArticyObjectToolTip::OnOpening()
{
	if(ObjectToDisplay.IsValid())
	{
		SetContentWidget(CreateToolTipContent());
	}
}

void SArticyObjectToolTip::OnClosed()
{
	SetContentWidget(SNullWidget::NullWidget);
}

TSharedRef<SWidget> SArticyObjectToolTip::CreateToolTipContent()
{
	const FString AssetName = ObjectToDisplay.Get()->GetName();
	const UClass* ClassOfObject = ObjectToDisplay.Get()->UObject::GetClass();

	// The tooltip contains the name, class, path, and asset registry tags
	// Use asset name by default, overwrite with display name where it makes sense
	FText NameText = FText::FromString(AssetName);
	const FText ClassText = FText::Format(LOCTEXT("ClassName", "({0})"), FText::FromString(ClassOfObject->GetName()));

	// Create a box to hold every line of info in the body of the tooltip
	TSharedRef<SVerticalBox> InfoBox = SNew(SVerticalBox);

	// overwrite the asset name with the display name
	bool bUsingDisplayName = false;
	IArticyObjectWithDisplayName* ArticyObjectWithDisplayName = Cast<IArticyObjectWithDisplayName>(ObjectToDisplay);
	if (ArticyObjectWithDisplayName)
	{
		const FText DisplayName = ArticyObjectWithDisplayName->GetDisplayName();
		if(!DisplayName.IsEmpty())
		{
			NameText = DisplayName;
			bUsingDisplayName = true;
		}
	}

	IArticyObjectWithSpeaker* ArticyObjectWithSpeaker = Cast<IArticyObjectWithSpeaker>(ObjectToDisplay);
	if(ArticyObjectWithSpeaker)
	{
		const UArticyObject* Speaker = UArticyObject::FindAsset(ArticyObjectWithSpeaker->GetSpeakerId());
		const IArticyObjectWithDisplayName* DisplayNameOfSpeaker = Cast<IArticyObjectWithDisplayName>(Speaker);
		const FText DisplayName = DisplayNameOfSpeaker->GetDisplayName();
		
		AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipSpeaker", "Speaker"), DisplayName, true);
	}
	
	// add the text to the tooltip body if possible
	IArticyObjectWithText* ArticyObjectWithText = Cast<IArticyObjectWithText>(ObjectToDisplay);
	if (ArticyObjectWithText)
	{
		const FText& Text = ArticyObjectWithText->GetText();
		if (Text.IsEmpty())
		{
			// if there is empty text, add no text at all - maybe "..." or "Empty"?
		}
		else
		{
			AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipText", "Text"), FText::FromString(FString("\"").Append(Text.ToString()).Append("\"")), true);
		}
	}


	// if we overwrote the asset name with the display name, attach the asset name in the tooltip body
	if (bUsingDisplayName)
	{
		AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipAssetName", "Asset Name"), FText::FromString(AssetName), false);
	}

	// add class name
	AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipClass", "Class"), ClassText, false);

	// Add Path
	//AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipPath", "Path"), FText::FromName(AssetData.PackagePath), false);

	TSharedRef<SVerticalBox> OverallTooltipVBox = SNew(SVerticalBox);

	// Top section (asset name, type, is checked out)
	OverallTooltipVBox->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 4)
		[
			SNew(SBorder)
			.Padding(6)
			.BorderImage(FEditorStyle::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.Padding(0, 0, 4, 0)
					[
						SNew(STextBlock)
						.Text(NameText)
						.Font(FEditorStyle::GetFontStyle("ContentBrowser.TileViewTooltip.NameFont"))
						.AutoWrapText(true)
					]
				]
			]
		];

	// Bottom section (additional information)
	OverallTooltipVBox->AddSlot()
	.AutoHeight()
	[		
		SNew(SBorder)
		.Padding(6)
		.BorderImage(FEditorStyle::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				InfoBox
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Top)
			.AutoWidth()
			[
				SNew(SBox)
				.Padding(FMargin(10, 2, 2, 2))
				[
					SNew(SImage)
					.Image(this, &SArticyObjectToolTip::GetTooltipImage)
				]
			]
		]			
	];

	

	return SNew(SBorder)
	.Padding(6)
	.BorderImage(FEditorStyle::GetBrush("ContentBrowser.TileViewTooltip.NonContentBorder"))
	[
		SNew(SBox)
		.MaxDesiredWidth(500.f)
		[
			OverallTooltipVBox
		]
		
	];
}

void SArticyObjectToolTip::AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, const FText& Value, bool bImportant) const
{
	FWidgetStyle ImportantStyle;
	ImportantStyle.SetForegroundColor(FLinearColor(1, 0.5, 0, 1));

	InfoBox->AddSlot()
		.AutoHeight()
		.Padding(0, 1)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0, 0, 4, 0)
			[
				SNew(STextBlock).Text(FText::Format(LOCTEXT("AssetViewTooltipFormat", "{0}:"), Key))
				.ColorAndOpacity(bImportant ? ImportantStyle.GetSubduedForegroundColor() : FSlateColor::UseSubduedForeground())
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock).Text(Value)
				.WrapTextAt(400)
				.ColorAndOpacity(bImportant ? ImportantStyle.GetForegroundColor() : FSlateColor::UseSubduedForeground())
				.HighlightText((Key.ToString() == TEXT("Path")) ? HighlightText : FText())
				.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
			]
		];
}

const FSlateBrush* SArticyObjectToolTip::GetTooltipImage() const
{
	return &TooltipBrush;
}

#undef LOCTEXT_NAMESPACE
