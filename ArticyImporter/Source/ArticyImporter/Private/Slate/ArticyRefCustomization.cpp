//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "Slate/ArticyRefCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/SWidget.h"
#include "Modules/ModuleManager.h"
#include "IDetailPropertyRow.h"
#include "Delegates/Delegate.h"
#include "ArticyObject.h"
#include "ArticyRef.h"
#include "ClassViewerModule.h"
#include "ArticyImporterStyle.h"

TSharedRef<IPropertyTypeCustomization> FArticyRefCustomization::MakeInstance()
{
	return MakeShareable(new FArticyRefCustomization());
}

void FArticyRefCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ArticyRefPropertyHandle = PropertyHandle;

	// update the reference upon selecting the ref; this only serves cosmetic purposes. The underlying Id will not be changed
	FArticyRef* ArticyRef = RetrieveArticyRef(ArticyRefPropertyHandle.Get());

	UArticyObject* SelectedObject = UArticyObject::FindAsset(ArticyRef->GetId());
	// set the selected class to the currently selected object because the class selection widget has no actual property as a basis to serialize and keep its state
	if (SelectedObject)
	{
		ClassRestriction = SelectedObject->UObject::GetClass();
	}

	if (ClassRestriction == nullptr)
	{
		ClassRestriction = UArticyObject::StaticClass();
	}
	ArticyRefProperty = SNew(SArticyRefProperty, ArticyRefPropertyHandle.Get())
		.ClassRestriction(this, &FArticyRefCustomization::GetClassRestriction);

	HeaderRow.NameContent()
	[
		ArticyRefPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(150)
	[
		ArticyRefProperty.ToSharedRef()
	];
}

void FArticyRefCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ClassRestrictionButton =
		SNew(SComboButton)
		.OnGetMenuContent(this, &FArticyRefCustomization::CreateClassPicker)
		.ContentPadding(2.f)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &FArticyRefCustomization::GetChosenClassName)
		];

	ChildBuilder.AddCustomRow(FText::FromString("Class Restriction"))
		.NameContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(0, 1, 0, 1))
			.FillWidth(1)
			[
				SNew(SBorder)
				.Padding(FMargin(0.0f, 2.0f))
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Class Restriction"))
				]
			]
		]
		.ValueContent()
		.MinDesiredWidth(150.f)
		.MaxDesiredWidth(250.f)
		[
			ClassRestrictionButton.ToSharedRef()
		];
	
	uint32 NumChildren;
	ArticyRefPropertyHandle->GetNumChildren(NumChildren);

	// restore all default editor property widgets
	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedRef< IPropertyHandle > ChildHandle = ArticyRefPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();
		IDetailPropertyRow& Row = ChildBuilder.AddProperty(ChildHandle);

		// disable the Id property here so that the user can't manipulate the ArticyID directly
		// UProperty is not set to ReadOnly due to needing to be editable to access "SetValue" functions from the IPropertyHandle system
		if(ChildHandle->GetPropertyDisplayName().EqualTo(FText::FromString(TEXT("Id"))))
		{
			Row.IsEnabled(false);
		}
	}
}

FArticyRef* FArticyRefCustomization::RetrieveArticyRef(IPropertyHandle* ArticyRefHandle)
{
	FArticyRef* ArticyRef = nullptr;

#if ENGINE_MINOR_VERSION >=20
	void* ArticyRefAddress;
	ArticyRefHandle->GetValueData(ArticyRefAddress);
	ArticyRef = static_cast<FArticyRef*>(ArticyRefAddress);
#elif ENGINE_MINOR_VERSION == 19
	TArray<void*> Addresses;
	ArticyRefHandle->AccessRawData(Addresses);
	void* ArticyRefAddress = Addresses[0];
	ArticyRef = static_cast<FArticyRef*>(ArticyRefAddress);
#endif

	return ArticyRef;
}

UClass* FArticyRefCustomization::GetClassRestriction() const
{
	if (ClassRestriction)
	{
		return ClassRestriction;
	}

	return UArticyObject::StaticClass();
}

FText FArticyRefCustomization::GetChosenClassName() const
{
	if (ClassRestriction) {
		return FText::FromString(ClassRestriction->GetName());
	}

	return FText::FromString("None");
}

void FArticyRefCustomization::OnClassPicked(UClass* InChosenClass)
{
	ClassRestriction = InChosenClass;
	ClassRestrictionButton->SetIsOpen(false, false);
}

TSharedRef<SWidget> FArticyRefCustomization::CreateClassPicker() const
{
	FClassViewerInitializationOptions ClassViewerConfig;
	ClassViewerConfig.DisplayMode = EClassViewerDisplayMode::TreeView;
	ClassViewerConfig.bAllowViewOptions = true;
	ClassViewerConfig.ClassFilter = MakeShareable(new FArticyRefClassFilter);
	return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(ClassViewerConfig, FOnClassPicked::CreateRaw(this, &FArticyRefCustomization::OnClassPicked));
}

FArticyId FArticyRefCustomization::GetIdFromValueString(FString SourceString)
{
	int32 Low, High = 0;
	const bool bSuccess = FParse::Value(*SourceString, TEXT("Low="), Low) && FParse::Value(*SourceString, TEXT("High="), High);

	FArticyId Id;
	Id.High = High;
	Id.Low = Low;
	return Id;
}
