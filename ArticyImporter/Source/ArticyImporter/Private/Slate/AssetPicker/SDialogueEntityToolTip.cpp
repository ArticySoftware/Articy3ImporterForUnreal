// Fill out your copyright notice in the Description page of Project Settings.

#include "SDialogueEntityToolTip.h"
#include "ArticyFlowClasses.h"
#include <AssetRegistryModule.h>
#include "ArticyObjectWithDisplayName.h"
#include "ArticyObjectWithText.h"
#include <SBoxPanel.h>
#include <STextBlock.h>
#include <EditorStyleSet.h>
#include <SBorder.h>
#include <ModuleManager.h>
#include <SToolTip.h>
#include <Text.h>
#include "ArticyObjectWithSpeaker.h"

#define LOCTEXT_NAMESPACE "DialogueEntityToolTip"

void SDialogueEntityToolTip::Construct(const FArguments& InArgs)
{
	ObjectToDisplay = InArgs._ObjectToDisplay;


	SToolTip::Construct(
		SToolTip::FArguments()
		.TextMargin(1.f)
		.BorderImage(FEditorStyle::Get().GetBrush("ContentBrowser.TileViewTooltip.ToolTipBorder"))
		// Text makes tooltip show, probably because it doesn't initialize otherwise
		.Text(FText::FromString("TEST"))
		);

}

void SDialogueEntityToolTip::OnOpening()
{
	if(ObjectToDisplay.IsValid())
	{
		SetContentWidget(CreateToolTipContent());
	}
}

void SDialogueEntityToolTip::OnClosed()
{
	SetContentWidget(SNullWidget::NullWidget);
}

TSharedRef<SWidget> SDialogueEntityToolTip::CreateToolTipContent()
{
	FAssetRegistryModule& ARModule = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = ARModule.Get().GetAssetByObjectPath(FName(*ObjectToDisplay->GetPathName()));

	// The tooltip contains the name, class, path, and asset registry tags
	// Use asset name by default, overwrite with display name where it makes sense
	FText NameText = FText::FromName(AssetData.AssetName);
	const FText ClassText = FText::Format(LOCTEXT("ClassName", "({0})"), FText::FromName(*AssetData.GetClass()->GetName()));

	// Create a box to hold every line of info in the body of the tooltip
	TSharedRef<SVerticalBox> InfoBox = SNew(SVerticalBox);


	// overwrite the asset name with the display name
	bool bUsingDisplayName = false;
	IArticyObjectWithDisplayName* displayNameObject = Cast<IArticyObjectWithDisplayName>(ObjectToDisplay);
	if (displayNameObject)
	{
		FText displayName = displayNameObject->GetDisplayName();
		if(!displayName.IsEmpty())
		{
			NameText = displayName;
			bUsingDisplayName = true;
		}
	}

	IArticyObjectWithSpeaker* speakerObject = Cast<IArticyObjectWithSpeaker>(ObjectToDisplay);
	if(speakerObject)
	{
		// #TODO FOR ARTICY LATER
		// #CONCERN
		// GetSpeaker (articy) or ExtractSpeaker (mine) have to use the db classes
		// with just articy db, it accesses a copy, which doesn't work at editor time
		// Will need to change articy db so that it can access original files
		const UArticyObject* speaker = nullptr;
		FText displayName = FText::FromString("Extract speaker display name here");
		AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipSpeaker", "Speaker"), displayName, true);
	}
	
	// add the text to the tooltip body if possible
	IArticyObjectWithText* textObject = Cast<IArticyObjectWithText>(ObjectToDisplay);
	if (textObject)
	{
		const FText& text = textObject->GetText();
		if (text.IsEmpty())
		{
			// if there is empty text, add no text at all - maybe "..." or "Empty"?
		}
		else
		{
			AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipText", "Text"), FText::FromString(FString("\"").Append(text.ToString()).Append("\"")), true);
		}
	}


	// if we overwrote the asset name with the display name, attach the asset name in the tooltip body
	if (bUsingDisplayName)
	{
		AddToToolTipInfoBox(InfoBox, LOCTEXT("DialogueEntityToolTipAssetName", "Asset Name"), FText::FromName(AssetData.AssetName), false);
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

	// Bottom section (asset registry tags)
	OverallTooltipVBox->AddSlot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(6)
			.BorderImage(FEditorStyle::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
			[
				InfoBox
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

void SDialogueEntityToolTip::AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, const FText& Value, bool bImportant) const
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
				.WrapTextAt(450)
				.ColorAndOpacity(bImportant ? ImportantStyle.GetForegroundColor() : FSlateColor::UseSubduedForeground())
				.HighlightText((Key.ToString() == TEXT("Path")) ? HighlightText : FText())
				.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
			]
		];
}

#undef LOCTEXT_NAMESPACE