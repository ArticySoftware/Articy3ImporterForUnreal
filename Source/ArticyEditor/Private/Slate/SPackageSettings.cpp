//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/SPackageSettings.h"
#include "ArticyDatabase.h"
#include "ArticyPluginSettings.h"
#include "EditorStyleSet.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSplitter.h"


void SPackageSettings::Construct(const FArguments& InArgs)
{
	PackageToDisplay = InArgs._PackageToDisplay;
	
	this->ChildSlot
	[
		SNew(SSplitter)
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0
		.Style(FAppStyle::Get(), "DetailsView.Splitter")
#else
		.Style(FEditorStyle::Get(), "DetailsView.Splitter")
#endif
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
				.IsEnabled_Lambda([]()
				{
					return UArticyDatabase::GetMutableOriginal().IsValid();
				})
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
	TWeakObjectPtr<UArticyDatabase> ArticyDatabase = UArticyDatabase::GetMutableOriginal();
	ArticyDatabase->ChangePackageDefault(PackageToDisplay, bChecked);
	
	UArticyPluginSettings* settings = GetMutableDefault<UArticyPluginSettings>();
	settings->PackageLoadSettings.Add(PackageToDisplay.ToString(), bChecked);
}

ECheckBoxState SPackageSettings::IsChecked() const
{
	TWeakObjectPtr<UArticyDatabase> ArticyDatabase = UArticyDatabase::GetMutableOriginal();
	if(!ArticyDatabase.IsValid())
	{
		return ECheckBoxState::Undetermined;
	}
	
	const bool bIsDefaultPackage = ArticyDatabase->IsPackageDefaultPackage(PackageToDisplay.ToString());
	
	return bIsDefaultPackage ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}


