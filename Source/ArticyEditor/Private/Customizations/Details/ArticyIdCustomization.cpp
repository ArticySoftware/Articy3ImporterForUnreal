//		
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Customizations/Details/ArticyIdCustomization.h"
#include "ArticyFunctionLibrary.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "ArticyObject.h"
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

	if(ArticyIdPropertyHandle->HasMetaData("ArticyNoWidget"))
	{
		bShouldCustomize = !ArticyIdPropertyHandle->GetBoolMetaData("ArticyNoWidget");
	}

	if(!bShouldCustomize)
	{
		HeaderRow.NameContent()
		[
			ArticyIdPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			ArticyIdPropertyHandle->CreatePropertyValueWidget()
		];
		return;
	}

	ArticyIdPropertyWidget = SNew(SArticyIdProperty)
	.ArticyIdToDisplay(this, &FArticyIdCustomization::GetArticyId)
	.OnArticyIdChanged(this, &FArticyIdCustomization::OnArticyIdChanged)
	.TopLevelClassRestriction(this, &FArticyIdCustomization::GetClassRestrictionMetaData)
	.bExactClass(IsExactClass())
	.bExactClassEditable(HasExactClassMetaData())
	.bIsReadOnly(this, &FArticyIdCustomization::IsReadOnly);

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
	// dont do children
}

FArticyId* FArticyIdCustomization::RetrieveArticyId(IPropertyHandle* ArticyIdHandle)
{
	FArticyId* ArticyId = nullptr;
	void* ArticyRefAddress;
	ArticyIdHandle->GetValueData(ArticyRefAddress);
	ArticyId = static_cast<FArticyId*>(ArticyRefAddress);
	return ArticyId;
}

FArticyId FArticyIdCustomization::GetArticyId() const
{
	FArticyId* ArticyId = RetrieveArticyId(ArticyIdPropertyHandle.Get());
	return ArticyId ? *ArticyId : FArticyId();
}

void FArticyIdCustomization::OnArticyIdChanged(const FArticyId &NewArticyId) const
{
	// update the articy ref with the new ID:
	// done via Set functions instead of accessing the ref object directly because using "Set" handles various Unreal logic, such as:
	// - CDO default change forwarding to instances
	// - marking dirty
	// - transaction buffer (Undo, Redo)
	ArticyIdPropertyHandle->SetValueFromFormattedString(NewArticyId.ToString());
}

UClass* FArticyIdCustomization::GetClassRestrictionMetaData() const
{
	UClass* Restriction = nullptr;

	if(HasClassRestrictionMetaData())
	{
		const FString ArticyClassRestriction = ArticyIdPropertyHandle->GetMetaData("ArticyClassRestriction");

		auto FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), TEXT("ArticyRuntime"), *ArticyClassRestriction);
		Restriction = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, UArticyObject::StaticClass());

		// the class name can be in the ArticyRuntime module or in the project module. If it wasn't found in ArticyRuntime, check the project module
		if(Restriction == nullptr)
		{
			FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), FApp::GetProjectName(), *ArticyClassRestriction);
			Restriction = ConstructorHelpersInternal::FindOrLoadClass(FullClassName, UArticyObject::StaticClass());
		}
	}

	if(Restriction == nullptr)
	{
		Restriction = UArticyObject::StaticClass();
	}

	return Restriction;
}

bool FArticyIdCustomization::HasClassRestrictionMetaData() const
{
	return ArticyIdPropertyHandle->HasMetaData("ArticyClassRestriction");
}

bool FArticyIdCustomization::IsExactClass() const
{
	if(HasExactClassMetaData())
	{
		return ArticyIdPropertyHandle->GetBoolMetaData("ArticyExactClass");
	}

	return false;
}

bool FArticyIdCustomization::IsReadOnly() const
{
	return ArticyIdPropertyHandle->GetNumPerObjectValues() != 1 || ArticyIdPropertyHandle->IsEditConst();
}

bool FArticyIdCustomization::HasExactClassMetaData() const
{
	return ArticyIdPropertyHandle->HasMetaData("ArticyExactClass");
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
