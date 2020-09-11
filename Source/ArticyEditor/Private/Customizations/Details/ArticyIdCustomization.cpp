//		
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Customizations/Details/ArticyIdCustomization.h"
#include "ArticyFunctionLibrary.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/SWidget.h"
#include "Modules/ModuleManager.h"
#include "IDetailPropertyRow.h"
#include "Delegates/Delegate.h"
#include "ArticyObject.h"
#include "ArticyRef.h"
#include "ClassViewerModule.h"
#include "UObject/ConstructorHelpers.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "EditorCategoryUtils.h"

FArticyRefClassFilter::FArticyRefClassFilter(UClass* InGivenClass, bool bInRequiresExactClass) : GivenClass(InGivenClass), bRequiresExactClass(bInRequiresExactClass)
{
}

TSharedRef<IPropertyTypeCustomization> FArticyIdCustomization::MakeInstance()
{
	return MakeShareable(new FArticyIdCustomization());
}

void FArticyIdCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ArticyIdPropertyHandle = PropertyHandle;
	
	bIsEditable = PropertyHandle->GetNumPerObjectValues() == 1;

	ArticyIdPropertyWidget = SNew(SArticyIdProperty)
		.ShownObject(this, &FArticyIdCustomization::GetArticyId)
		.OnArticyObjectSelected(this, &FArticyIdCustomization::SetAsset)
		.TopLevelClassRestriction(this, &FArticyIdCustomization::GetClassRestrictionMetaData)
		.bExactClass(IsExactClass())
		.bExactClassEditable(HasExactClassMetaData())
		.IsEnabled(bIsEditable);

	if(bIsEditable)
	{
		// update the reference upon selecting the ref; this only serves cosmetic purposes. The underlying Id will not be changed
		FString ValueString;
		const FPropertyAccess::Result Result = ArticyIdPropertyHandle->GetValueAsFormattedString(ValueString);

		FArticyId Id = FArticyId();
		if (Result == FPropertyAccess::Success)
		{
			Id.InitFromString(ValueString);
		}
		
		UArticyObject* SelectedObject = UArticyObject::FindAsset(Id);

		// attempt to set the class restriction via meta data (cpp means that the programmer has mandated a specific type)
		ClassRestriction = GetClassRestrictionMetaData();
		
		// if the class of the current object is within 
		if (SelectedObject)
		{
			UClass* CurrentClass = SelectedObject->UObject::GetClass();
			if (CurrentClass->IsChildOf(ClassRestriction))
			{
				ClassRestriction = CurrentClass;
			}
		}
	}

	TAttribute<bool> EnableAttribute;
	EnableAttribute.Bind(this, &FArticyIdCustomization::IsEditable);
	HeaderRow.IsEnabled(EnableAttribute);
	
	HeaderRow.NameContent()
	[
		ArticyIdPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(150)
	[
		ArticyIdPropertyWidget.ToSharedRef()
	];
}

void FArticyIdCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{	
	uint32 NumChildren;
	ArticyIdPropertyHandle->GetNumChildren(NumChildren);

	// restore all default editor property widgets
	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedRef< IPropertyHandle > ChildHandle = ArticyIdPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();
		IDetailPropertyRow& Row = ChildBuilder.AddProperty(ChildHandle);

		// disable the Id property here so that the user can't manipulate the ArticyID directly
		// UProperty is not set to ReadOnly due to needing to be editable to access "SetValue" functions from the IPropertyHandle system
		if(!bIsEditable || ChildHandle->GetPropertyDisplayName().EqualTo(FText::FromString(TEXT("Id"))))
		{
			Row.IsEnabled(false);
		}
	}
}

FArticyId* FArticyIdCustomization::RetrieveArticyId(IPropertyHandle* ArticyIdHandle)
{
	FArticyId* ArticyId = nullptr;
	void* ArticyRefAddress;
	ArticyIdHandle->GetValueData(ArticyRefAddress);
	ArticyId = static_cast<FArticyId*>(ArticyRefAddress);
	return ArticyId;
}

UClass* FArticyIdCustomization::GetClassRestriction() const
{
	if (ClassRestriction)
	{
		return ClassRestriction;
	}

	return UArticyObject::StaticClass();
}

FArticyId FArticyIdCustomization::GetArticyId() const
{
	FArticyId* ArticyId = RetrieveArticyId(ArticyIdPropertyHandle.Get());
	return ArticyId ? *ArticyId : FArticyId();
}

void FArticyIdCustomization::SetAsset(const FAssetData& AssetData) const
{
	// retrieve the newly selected articy object
	const UArticyObject* NewSelection = Cast<UArticyObject>(AssetData.GetAsset());

	// if the new selection is not valid we cleared the selection
	const FArticyId NewId = NewSelection ? NewSelection->GetId() : FArticyId();

	// get the current articy ref struct as formatted string
	FString FormattedValueString;
	ArticyIdPropertyHandle->GetValueAsFormattedString(FormattedValueString);

	// remove the old ID string
	const int32 IdIndex = FormattedValueString.Find(FString(TEXT("Low=")), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	const int32 EndOfIdIndex = FormattedValueString.Find(FString(TEXT(")")), ESearchCase::IgnoreCase, ESearchDir::FromStart, IdIndex);
	FormattedValueString.RemoveAt(IdIndex, EndOfIdIndex - IdIndex);

	// reconstruct the value string with the new ID
	const FString NewIdString = FString::Format(TEXT("Low={0}, High={1}"), { NewId.Low, NewId.High, });
	FormattedValueString.InsertAt(IdIndex, *NewIdString);

	// update the articy ref with the new ID:
	// done via Set functions instead of accessing the ref object directly because using "Set" handles various Unreal logic, such as:
	// - CDO default change forwarding to instances
	// - marking dirty
	// - transaction buffer (Undo, Redo)
	ArticyIdPropertyHandle->SetValueFromFormattedString(FormattedValueString);
}

UClass* FArticyIdCustomization::GetClassRestrictionMetaData() const
{
	UClass* Restriction = UArticyObject::StaticClass();

	if(HasClassRestrictionMetaData())
	{
		const FString ArticyClassRestriction = ArticyIdPropertyHandle->GetMetaData(TEXT("ArticyClassRestriction"));

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

bool FArticyIdCustomization::HasClassRestrictionMetaData() const
{
	return ArticyIdPropertyHandle->HasMetaData(TEXT("ArticyClassRestriction"));
}

bool FArticyIdCustomization::IsExactClass() const
{
	if(HasExactClassMetaData())
	{
		return ArticyIdPropertyHandle->GetBoolMetaData(TEXT("ArticyExactClass"));
	}

	return false;
}

bool FArticyIdCustomization::HasExactClassMetaData() const
{
	return ArticyIdPropertyHandle->HasMetaData(TEXT("ArticyExactClass"));
}

FArticyId FArticyIdCustomization::GetIdFromValueString(FString SourceString)
{
	int32 Low, High = 0;
	const bool bSuccess = FParse::Value(*SourceString, TEXT("Low="), Low) && FParse::Value(*SourceString, TEXT("High="), High);

	FArticyId Id;
	Id.High = High;
	Id.Low = Low;
	return Id;
}
