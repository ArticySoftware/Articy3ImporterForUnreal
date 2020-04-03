//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "Slate/SPackageSettings.h"
#include "ArticyDatabase.h"
#include "ArticyPluginSettings.h"
#include "Widgets/Layout/SSplitter.h"
#include "EditorStyle.h"


void SPackageSettings::Construct(const FArguments& InArgs)
{
	PackageToDisplay = InArgs._PackageToDisplay;
	
	this->ChildSlot
	[
		SNew(SSplitter)
		.Style(FEditorStyle::Get(), "DetailsView.Splitter")
		.PhysicalSplitterHandleSize(1.0f)
		.HitDetectionSplitterHandleSize(5.0f)
		+ SSplitter::Slot().Value(0.328f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.Text(this, &SPackageSettings::GetPackageName)
			]
		]
		+ SSplitter::Slot().Value(0.672f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.Padding(FMargin(5, 0, 0, 5))
			[
				SNew(SCheckBox)
				.OnCheckStateChanged(this, &SPackageSettings::OnCheckStateChanged)
				.IsChecked(this, &SPackageSettings::IsChecked)
			]
		]
	]; 

}

FText SPackageSettings::GetPackageName() const
{
	return FText::FromName(PackageToDisplay);
}

void SPackageSettings::OnCheckStateChanged(ECheckBoxState NewState) const
{
	const bool bChecked = NewState == ECheckBoxState::Checked ? true : false;
	UArticyDatabase* originalDatabase = UArticyDatabase::GetMutableOriginal();
	originalDatabase->ChangePackageDefault(PackageToDisplay, bChecked);
	
	UArticyPluginSettings* settings = GetMutableDefault<UArticyPluginSettings>();
	settings->PackageLoadSettings.Add(PackageToDisplay.ToString(), bChecked);
}

ECheckBoxState SPackageSettings::IsChecked() const
{
	UArticyDatabase* originalDatabase = UArticyDatabase::GetMutableOriginal();
	const bool bIsDefaultPackage = originalDatabase->IsPackageDefaultPackage(PackageToDisplay.ToString());
	
	return bIsDefaultPackage ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}


