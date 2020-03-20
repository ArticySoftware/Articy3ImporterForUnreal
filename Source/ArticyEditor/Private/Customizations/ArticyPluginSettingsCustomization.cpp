//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "Customizations/ArticyPluginSettingsCustomization.h"

#include "ArticyEditorFunctionLibrary.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "ArticyDatabase.h"
#include "Slate/SPackageSettings.h"
#include "ArticyEditorModule.h"

#define LOCTEXT_NAMESPACE "ArticyPluginSettings"

FArticyPluginSettingsCustomization::FArticyPluginSettingsCustomization()
{

}

FArticyPluginSettingsCustomization::~FArticyPluginSettingsCustomization()
{
	// closing the settings window means we no longer want to refresh the UI
	FArticyEditorModule& ArticyEditorModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>("ArticyEditor");
	ArticyEditorModule.OnImportFinished.Remove(RefreshHandle);
}

TSharedRef<IDetailCustomization> FArticyPluginSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FArticyPluginSettingsCustomization);
}

void FArticyPluginSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	LayoutBuilder = &DetailLayout;

	// after importing, refresh the custom UI
	FArticyEditorModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>("ArticyEditor");
	RefreshHandle = ArticyImporterModule.OnImportFinished.AddRaw(this, &FArticyPluginSettingsCustomization::RefreshSettingsUI);

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
	FArticyEditorModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>("ArticyEditor");
	ArticyImporterModule.OnImportFinished.Remove(RefreshHandle);
	
}

#undef LOCTEXT_NAMESPACE
