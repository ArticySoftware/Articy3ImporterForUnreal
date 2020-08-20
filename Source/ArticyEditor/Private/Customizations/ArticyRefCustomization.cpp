//		
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Customizations/ArticyRefCustomization.h"
#include "ArticyEditorStyle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/SWidget.h"
#include "Modules/ModuleManager.h"
#include "IDetailPropertyRow.h"
#include "Delegates/Delegate.h"
#include "ArticyObject.h"
#include "ArticyRef.h"
#include "ClassViewerModule.h"
#include "Interfaces/ArticyObjectWithText.h"
#include "UObject/ConstructorHelpers.h"
#include "Slate/UserInterfaceHelperFunctions.h"

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

	// attempt to set the class restriction via meta data (cpp means that the programmer has mandated a specific type)
	ClassRestriction = GetClassRestrictionMetaData();
	
	// if there is no meta data class restriction, 
	if (ClassRestriction == nullptr && SelectedObject)
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
		.IsEnabled_Lambda([this]() -> bool
	{
		return !HasClassRestrictionMetaData();
	})
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
	void* ArticyRefAddress;
	ArticyRefHandle->GetValueData(ArticyRefAddress);
	ArticyRef = static_cast<FArticyRef*>(ArticyRefAddress);
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

TSharedRef<SWidget> FArticyRefCustomization::CreateClassPicker()
{
	FClassViewerInitializationOptions ClassViewerConfig;
	ClassViewerConfig.DisplayMode = EClassViewerDisplayMode::TreeView;
	ClassViewerConfig.bAllowViewOptions = true;
	ClassViewerConfig.ClassFilter = MakeShareable(new FArticyRefClassFilter);
	return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(ClassViewerConfig, FOnClassPicked::CreateRaw(this, &FArticyRefCustomization::OnClassPicked));
}

UClass* FArticyRefCustomization::GetClassRestrictionMetaData() const
{
	UClass* Restriction = nullptr;

	if(HasClassRestrictionMetaData())
	{
		const FString ArticyClassRestriction = ArticyRefPropertyHandle->GetMetaData(TEXT("ArticyClassRestriction"));

		auto FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), TEXT("ArticyRuntime"), *ArticyClassRestriction);
		Restriction = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, UArticyObject::StaticClass());

		// the class name can be in the ArticyRuntime module or in the project module. If it wasn't found in ArticyRuntime, check the project module
		if(Restriction == nullptr)
		{
			FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), FApp::GetProjectName(), *ArticyClassRestriction);
			Restriction = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, UArticyObject::StaticClass());
		}
	}

	return Restriction;
}

bool FArticyRefCustomization::HasClassRestrictionMetaData() const
{
	return ArticyRefPropertyHandle->HasMetaData(TEXT("ArticyClassRestriction"));
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
