// Fill out your copyright notice in the Description page of Project Settings.

#include "ArticyImporterPrivatePCH.h"
#include "ArticyPluginSettingsCustomization.h"

#include "ArticyImporterFunctionLibrary.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"

#define LOCTEXT_NAMESPACE "ArticyPluginSettings"

FArticyPluginSettingsCustomization::FArticyPluginSettingsCustomization()
{

}

TSharedRef<IDetailCustomization> FArticyPluginSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FArticyPluginSettingsCustomization);
}

void FArticyPluginSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	IDetailCategoryBuilder& ImportActionsCategory = DetailLayout.EditCategory("ImportActions");

	ImportActionsCategory.AddCustomRow(LOCTEXT("ForceCompleteReimport_Row", ""))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("ForceCompleteReimport", "Force complete reimport"))
				.ToolTipText(LOCTEXT("ForceCompleteReimport_Tooltip", "Performs a complete reimport and asset regeneration."))
				.OnClicked_Lambda([]()->FReply { FArticyImporterFunctionLibrary::ForceCompleteReimport(); return FReply::Handled(); })
			]
		];

	ImportActionsCategory.AddCustomRow(LOCTEXT("ReimportChanges_Row", ""))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("ReimportChanges", "Reimport changes"))
			.ToolTipText(LOCTEXT("ReimportChanges_Tooltip", "Reimports data from the json file and regenerates assets. Does not reimport GVs and ObjectDefinitions if the hash is still the same."))
			.OnClicked_Lambda([]()->FReply { FArticyImporterFunctionLibrary::ReimportChanges(); return FReply::Handled(); })
			]
		];

	ImportActionsCategory.AddCustomRow(LOCTEXT("RegenerateAssets_Row", ""))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("RegenerateAssets", "Regenerate Assets"))
				.ToolTipText(LOCTEXT("RegenerateAssets_Tooltip", "Clears and regenerates all assets, but no code."))
				.OnClicked_Lambda([]()->FReply { FArticyImporterFunctionLibrary::RegenerateAssets(); return FReply::Handled(); })
			]
		];
}
