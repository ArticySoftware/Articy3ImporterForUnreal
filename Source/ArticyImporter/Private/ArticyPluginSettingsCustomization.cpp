//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyPluginSettingsCustomization.h"

#include "ArticyImporterFunctionLibrary.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "ArticyDatabase.h"
#include "Slate/SPackageSettings.h"
#include "ArticyImporter.h"

#define LOCTEXT_NAMESPACE "ArticyPluginSettings"

FArticyPluginSettingsCustomization::FArticyPluginSettingsCustomization()
{

}

FArticyPluginSettingsCustomization::~FArticyPluginSettingsCustomization()
{
	// closing the settings window means we no longer want to refresh the UI
	FArticyImporterModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyImporterModule>("ArticyImporter");
	ArticyImporterModule.OnImportFinished.Remove(RefreshHandle);
}

TSharedRef<IDetailCustomization> FArticyPluginSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FArticyPluginSettingsCustomization);
}

void FArticyPluginSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	LayoutBuilder = &DetailLayout;

	// after importing, refresh the custom UI
	FArticyImporterModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyImporterModule>("ArticyImporter");
	RefreshHandle = ArticyImporterModule.OnImportFinished.AddRaw(this, &FArticyPluginSettingsCustomization::RefreshSettingsUI);
	
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


	const UArticyDatabase* OriginalDatabase = UArticyDatabase::GetMutableOriginal();
	
	if (!OriginalDatabase) {

		// if there was no database found, check if we are still loading assets; if we are, refresh the custom UI once it's done
		FAssetRegistryModule& AssetRegistry = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");

		if (AssetRegistry.Get().IsLoadingAssets()) {
			AssetRegistry.Get().OnFilesLoaded().AddSP(this, &FArticyPluginSettingsCustomization::RefreshSettingsUI);
		}
		
		return;
	}
	IDetailCategoryBuilder& DefaultPackagesCategory= DetailLayout.EditCategory("Default packages");

	TArray<TSharedPtr<SPackageSettings>> PackageSettingsWidgets;

	// create a custom widget per package
	for(FString PackageName : OriginalDatabase->GetImportedPackageNames())
	{
		const FName PackageNameAsName = FName(*PackageName);
		TSharedPtr<SPackageSettings> NewSettingsWidget = 
			SNew(SPackageSettings)
			.PackageToDisplay(PackageNameAsName);

		PackageSettingsWidgets.Add(NewSettingsWidget);
	}

	// add the custom widgets to the UI
	for (TSharedPtr<SPackageSettings> PackageSettingsWidget : PackageSettingsWidgets)
	{
		DefaultPackagesCategory.AddCustomRow(LOCTEXT("PackageSetting", ""))
		[
			PackageSettingsWidget.ToSharedRef()
		];
	}
}

void FArticyPluginSettingsCustomization::RefreshSettingsUI()
{
	ensure(LayoutBuilder);
	
	LayoutBuilder->ForceRefreshDetails();
	// the refresh will cause a new instance to be created and used, therefore clear the outdated refresh delegate handle
	FArticyImporterModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyImporterModule>("ArticyImporter");
	ArticyImporterModule.OnImportFinished.Remove(RefreshHandle);
	
}

#undef LOCTEXT_NAMESPACE
