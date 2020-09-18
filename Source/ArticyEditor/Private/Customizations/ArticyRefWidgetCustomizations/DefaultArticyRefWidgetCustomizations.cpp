//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.
//

#include "Customizations/ArticyRefWidgetCustomizations/DefaultArticyRefWidgetCustomizations.h"
#include "ArticyEditorStyle.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "ArticyObject.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

void FArticyButtonCustomization::RegisterArticyRefWidgetCustomization(FArticyRefWidgetCustomizationBuilder& Builder)
{
	ArticyObject = Builder.GetArticyObject();

	FArticyRefWidgetCustomizationInfo Info;
	Info.ExtraButtonExtender = MakeShared<FExtender>();
	Info.ExtraButtonExtender->AddToolBarExtension(TEXT("Base"), EExtensionHook::After, nullptr,
		FToolBarExtensionDelegate::CreateRaw(this, &FArticyButtonCustomization::CreateArticyButton));

	Builder.AddCustomization(Info);
}

void FArticyButtonCustomization::UnregisterArticyRefWidgetCustomization()
{
	ArticyObject = nullptr;
}

void FArticyButtonCustomization::CreateArticyButton(FToolBarBuilder& Builder)
{
	const FSlateBrush* ArticyDraftLogo = FArticyEditorStyle::Get().GetBrush("ArticyImporter.ArticyDraft.16");

	TSharedRef<SButton> ArticyButton = SNew(SButton)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
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
	UserInterfaceHelperFunctions::ShowObjectInArticy(ArticyObject.Get());
	return FReply::Handled();
}

TSharedPtr<IArticyRefWidgetCustomization> FArticyButtonCustomizationFactory::CreateCustomization()
{
	return MakeShareable(new FArticyButtonCustomization);
}

bool FArticyButtonCustomizationFactory::SupportsType(const UArticyObject* ArticyObject)
{
	if(!ArticyObject)
	{
		return false;
	}

	// supports all articy objects, hence return true. Modify this in your own override for more advanced customization
	return true;
}



