#include "SPackageSettings.h"
#include "ArticyDatabase.h"
#include "ArticyPluginSettings.h"


void SPackageSettings::Construct(const FArguments& InArgs)
{
	PackageToDisplay = InArgs._PackageToDisplay;
	
	this->ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(this, &SPackageSettings::GetPackageName)
		]

		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &SPackageSettings::OnCheckStateChanged)
			.IsChecked(this, &SPackageSettings::IsChecked)
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


