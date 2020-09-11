//		
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Customizations/Details/ArticyRefCustomization.h"
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
#include "Customizations/Details/ArticyIdCustomization.h"

TSharedRef<IPropertyTypeCustomization> FArticyRefCustomization::MakeInstance()
{
	return MakeShareable(new FArticyRefCustomization());
}

void FArticyRefCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ArticyRefPropertyHandle = PropertyHandle;
	
	bIsEditable = PropertyHandle->GetNumPerObjectValues() == 1;

	TAttribute<bool> EnableAttribute;
	EnableAttribute.Bind(this, &FArticyRefCustomization::IsEditable);
	HeaderRow.IsEnabled(EnableAttribute);
;
	ArticyIdPropertyWidget = SNew(SArticyIdProperty)
	.ShownObject(this, &FArticyRefCustomization::GetArticyId)
	.OnArticyObjectSelected(this, &FArticyRefCustomization::SetAsset)
	.TopLevelClassRestriction(this, &FArticyRefCustomization::GetClassRestrictionMetaData)
	.bExactClass(IsExactClass())
	.bExactClassEditable(!HasExactClassMetaData())
	.bClassFilterEditable(!IsExactClass())
	.IsEnabled(bIsEditable);
	
	HeaderRow.NameContent()
	[
		ArticyRefPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(150)
	[
		ArticyIdPropertyWidget.ToSharedRef()
	];
}

void FArticyRefCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{	
	uint32 NumChildren;
	ArticyRefPropertyHandle->GetNumChildren(NumChildren);

	// restore all default editor property widgets
	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedRef< IPropertyHandle > ChildHandle = ArticyRefPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();
		if(ChildHandle->GetPropertyDisplayName().EqualTo(FText::FromString("Id")))
		{
			//continue;
		}
		
		IDetailPropertyRow& Row = ChildBuilder.AddProperty(ChildHandle);

		// disable the Id property here so that the user can't manipulate the ArticyID directly
		// UProperty is not set to ReadOnly due to needing to be editable to access "SetValue" functions from the IPropertyHandle system
		if(!bIsEditable || ChildHandle->GetPropertyDisplayName().EqualTo(FText::FromString(TEXT("Id"))))
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

FArticyId FArticyRefCustomization::GetArticyId() const
{
	FArticyRef* ArticyRef = RetrieveArticyRef(ArticyRefPropertyHandle.Get());
	return ArticyRef ? ArticyRef->GetId() : FArticyId();
}

void FArticyRefCustomization::SetAsset(const FAssetData& AssetData) const
{
	// retrieve the newly selected articy object
	const UArticyObject* NewSelection = Cast<UArticyObject>(AssetData.GetAsset());

	// if the new selection is not valid we cleared the selection
	const FArticyId NewId = NewSelection ? NewSelection->GetId() : FArticyId();

	// get the current articy ref struct as formatted string
	FString FormattedValueString;
	ArticyRefPropertyHandle->GetValueAsFormattedString(FormattedValueString);

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
	ArticyRefPropertyHandle->SetValueFromFormattedString(FormattedValueString);
}

UClass* FArticyRefCustomization::GetClassRestrictionMetaData() const
{
	UClass* Restriction = UArticyObject::StaticClass();

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

bool FArticyRefCustomization::IsExactClass() const
{
	if(HasExactClassMetaData())
	{
		return ArticyRefPropertyHandle->GetBoolMetaData(TEXT("ArticyExactClass"));
	}

	return false;
}

bool FArticyRefCustomization::HasExactClassMetaData() const
{
	return ArticyRefPropertyHandle->HasMetaData(TEXT("ArticyExactClass"));
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
