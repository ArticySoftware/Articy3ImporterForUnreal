//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.
//


#include "Customizations/ArticyRefWidgetCustomizations/DefaultArticyRefWidgetCustomizations.h"
#include "ArticyEditorStyle.h"
#include "Slate/UserInterfaceHelperFunctions.h"

void FArticyButtonCustomization::RegisterArticyRefWidgetCustomization(FArticyRefWidgetCustomizationBuilder& Builder)
{
	Ref = &Builder.GetArticyRef();

	FArticyRefWidgetCustomizationInfo Info;
	Info.ExtraButtonExtender = MakeShared<FExtender>();
	Info.ExtraButtonExtender->AddToolBarExtension(TEXT("Base"), EExtensionHook::After, nullptr,
		FToolBarExtensionDelegate::CreateRaw(this, &FArticyButtonCustomization::CreateArticyButton));

	Builder.AddCustomization(Info);
}

void FArticyButtonCustomization::UnregisterArticyRefWidgetCustomization()
{
	Ref = nullptr;
}

void FArticyButtonCustomization::CreateArticyButton(FToolBarBuilder& Builder)
{
	const FSlateBrush* ArticyDraftLogo = FArticyEditorStyle::Get().GetBrush("ArticyImporter.ArticyDraft.16");

	TSharedRef<SButton> ArticyButton = SNew(SButton)
		.OnClicked(FOnClicked::CreateRaw(this, &FArticyButtonCustomization::OnArticyButtonClicked))
		.ToolTipText(FText::FromString("Show selected object in articy:draft"))
		.Content()
		[
			SNew(SImage)
			.Image(ArticyDraftLogo)
		];

	Builder.AddWidget(ArticyButton);
}

FReply FArticyButtonCustomization::OnArticyButtonClicked()
{
	UserInterfaceHelperFunctions::ShowObjectInArticy(Ref->GetId());
	return FReply::Handled();
}



