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

	UObject* Reference;
	EntityPropertyHandle->GetValue(Reference);

	void* refPointer;
	ArticyRefPropertyHandle->GetValueData(refPointer);

	FArticyRef* ref = static_cast<FArticyRef*>(refPointer);
	ref->GetReference();

	HeaderRow.NameContent()
	[
		ArticyRefPropertyHandle->CreatePropertyNameWidget()
	];

	

	EntityPropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FArticyRefCustomization::OnReferenceUpdated));

}

void FArticyRefCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	uint32 NumChildren;
	ArticyRefPropertyHandle->GetNumChildren(NumChildren);

	for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const TSharedRef< IPropertyHandle > ChildHandle = ArticyRefPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();

		//ChildBuilder.AddProperty(ChildHandle);
	}

	TSharedRef<IPropertyHandle> EntityPropertyHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Reference")).ToSharedRef();
	TSharedRef<IPropertyHandle> IdHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Id")).ToSharedRef();


	check(EntityPropertyHandle->IsValidHandle());

	UObject* Reference;
	EntityPropertyHandle->GetValue(Reference);

	TSharedRef<SWidget> entityWidget = EntityPropertyHandle->CreatePropertyValueWidget(true);
		

	ChildBuilder.AddCustomRow((FText::FromString("Reference")))
		.ValueContent()
		.MinDesiredWidth(150.f)
		.MaxDesiredWidth(250.f)
		[
			entityWidget
		];

	ChildBuilder.AddProperty(IdHandle);	
}

void FArticyRefCustomization::OnReferenceUpdated()
{
	UE_LOG(LogArticyImporter, Warning, TEXT("value changed"));

	TSharedRef<IPropertyHandle> EntityPropertyHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Reference")).ToSharedRef();
	check(EntityPropertyHandle->IsValidHandle());
	UObject* Reference;
	EntityPropertyHandle->GetValue(Reference);
	
	TSharedRef<IPropertyHandle> ArticyIdHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Id")).ToSharedRef();
	void* address;
	ArticyIdHandle->GetValueData(address);
	FArticyId* currentIdPointer = static_cast<FArticyId*>(address);
	// new asset selected
	if(Reference)
	{
		UArticyPrimitive* articyPrimitive = Cast<UArticyPrimitive>(Reference);

		UProperty * prop = ArticyIdHandle->GetProperty();
		UStructProperty* structProp = Cast<UStructProperty>(prop);
		
		FArticyId newId = articyPrimitive->GetId();

		currentIdPointer->High = newId.High;
		currentIdPointer->Low = newId.Low;
	}
	else
	{
		currentIdPointer->High = 0;
		currentIdPointer->Low = 0;
	}
	
}

