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
	ArticyIdPropertyWidget = SNew(SArticyRefProperty)
	.ArticyRefToDisplay(this, &FArticyRefCustomization::GetArticyRef)
	.OnArticyRefChanged(this, &FArticyRefCustomization::OnArticyRefChanged)
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
	// dont do children
}

FArticyRef* FArticyRefCustomization::RetrieveArticyRef(IPropertyHandle* ArticyRefHandle)
{
	FArticyRef* ArticyRef = nullptr;
	void* ArticyRefAddress;
	ArticyRefHandle->GetValueData(ArticyRefAddress);
	ArticyRef = static_cast<FArticyRef*>(ArticyRefAddress);
	return ArticyRef;
}

FArticyRef FArticyRefCustomization::GetArticyRef() const
{
	FArticyRef* ArticyRef = RetrieveArticyRef(ArticyRefPropertyHandle.Get());
	return ArticyRef ? *ArticyRef : FArticyRef();
}

void FArticyRefCustomization::OnArticyRefChanged(const FArticyRef& NewArticyRef) const
{
	// update the articy ref with the new ID:
	// done via Set functions instead of accessing the ref object directly because using "Set" handles various Unreal logic, such as:
	// - CDO default change forwarding to instances
	// - marking dirty
	// - transaction buffer (Undo, Redo)
	ArticyRefPropertyHandle->SetValueFromFormattedString(NewArticyRef.ToString());
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
