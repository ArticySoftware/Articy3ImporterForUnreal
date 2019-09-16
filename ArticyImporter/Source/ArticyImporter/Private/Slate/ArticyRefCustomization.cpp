#include "ArticyRefCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "SWidget.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IPropertyChangeListener.h"
#include "Delegate.h"
#include "ArticyPrimitive.h"
#include "ArticyRef.h"


TSharedRef<IPropertyTypeCustomization> FArticyRefCustomization::MakeInstance()
{
	return MakeShareable(new FArticyRefCustomization());
}

void FArticyRefCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ArticyRefPropertyHandle = PropertyHandle;

	TSharedRef<IPropertyHandle> EntityPropertyHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Reference")).ToSharedRef();

	check(EntityPropertyHandle->IsValidHandle());

	UObject* AssetReference;
	EntityPropertyHandle->GetValue(AssetReference);

	void* ArticyRefAddress;
	ArticyRefPropertyHandle->GetValueData(ArticyRefAddress);

	// update the reference upon selecting the ref
	FArticyRef* ArticyRef = static_cast<FArticyRef*>(ArticyRefAddress);
	ArticyRef->GetReference();

	HeaderRow.NameContent()
	[
		ArticyRefPropertyHandle->CreatePropertyNameWidget()
	];

	// handle the articy ID whenever the asset selection changes
	EntityPropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FArticyRefCustomization::OnReferenceUpdated));
}

void FArticyRefCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	uint32 NumChildren;
	ArticyRefPropertyHandle->GetNumChildren(NumChildren);

	// restore all default editor property widgets
	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedRef< IPropertyHandle > ChildHandle = ArticyRefPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();
		ChildBuilder.AddProperty(ChildHandle);
	}
}

void FArticyRefCustomization::OnReferenceUpdated()
{
	const TSharedRef<IPropertyHandle> EntityPropertyHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Reference")).ToSharedRef();

	check(EntityPropertyHandle->IsValidHandle());

	UObject* Reference;
	EntityPropertyHandle->GetValue(Reference);
	
	const TSharedRef<IPropertyHandle> ArticyIdHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Id")).ToSharedRef();

	void* ArticyIdPtr;
	ArticyIdHandle->GetValueData(ArticyIdPtr);
	
	FArticyId* CurrentIdPointer = static_cast<FArticyId*>(ArticyIdPtr);
	// new asset selected
	if(Reference)
	{
		UArticyPrimitive* ArticyPrimitive = Cast<UArticyPrimitive>(Reference);
		const FArticyId NewId = ArticyPrimitive->GetId();

		CurrentIdPointer->High = NewId.High;
		CurrentIdPointer->Low = NewId.Low;
	}
	else
	{
		CurrentIdPointer->High = 0;
		CurrentIdPointer->Low = 0;
	}
	
}

