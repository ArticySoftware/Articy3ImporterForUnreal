#include "ArticyRefCustomization.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "SWidget.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IPropertyChangeListener.h"
#include "Delegate.h"
#include "ArticyPrimitive.h"
#include "ArticyObject.h"
#include "ArticyRef.h"


TSharedRef<IPropertyTypeCustomization> FArticyRefCustomization::MakeInstance()
{
	return MakeShareable(new FArticyRefCustomization());
}

void FArticyRefCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	//ArticyRefPropertyHandle = PropertyHandle;

	//TSharedRef<IPropertyHandle> TechnicalNameHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("TechnicalName")).ToSharedRef();
	//const TSharedRef<IPropertyHandle> EntityPropertyHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("Reference")).ToSharedRef();

	//check(TechnicalNameHandle->IsValidHandle());

	//UObject* AssetReference;
	//EntityPropertyHandle->GetValue(AssetReference);

	//// update the reference upon selecting the ref; this only serves cosmetic purposes. The underlying Id will not be changed
	//FArticyRef* ArticyRef = RetrieveArticyRef();
	//ArticyRef->UpdateReference();

	//HeaderRow.NameContent()
	//[
	//	ArticyRefPropertyHandle->CreatePropertyNameWidget()
	//];

	//// handle the articy ID whenever the asset selection changes
	//TechnicalNameHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FArticyRefCustomization::OnReferenceUpdated));
}

void FArticyRefCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	//uint32 NumChildren;
	//ArticyRefPropertyHandle->GetNumChildren(NumChildren);

	//// restore all default editor property widgets
	//for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	//{
	//	const TSharedRef< IPropertyHandle > ChildHandle = ArticyRefPropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();
	//	ChildBuilder.AddProperty(ChildHandle);
	//}
}

void FArticyRefCustomization::OnReferenceUpdated() const
{
	const TSharedRef<IPropertyHandle> TechnicalNameHandle = ArticyRefPropertyHandle->GetChildHandle(TEXT("TechnicalName")).ToSharedRef();

	check(TechnicalNameHandle->IsValidHandle());

	FString TechnicalName;
	TechnicalNameHandle->GetValue(TechnicalName);

	FArticyRef* ArticyRef = RetrieveArticyRef();
	
	// find the asset that matches the new technical name, and set it as the target
	UArticyObject * NewReference = UArticyObject::FindAsset(TechnicalName);
	ArticyRef->SetReference(NewReference);
}

FArticyRef* FArticyRefCustomization::RetrieveArticyRef() const
{
	FArticyRef* ArticyRef = nullptr;

#if ENGINE_MINOR_VERSION >=20
	void* ArticyRefAddress;
	ArticyRefPropertyHandle->GetValueData(ArticyRefAddress);
	ArticyRef = static_cast<FArticyRef*>(ArticyRefAddress);
#elif ENGINE_MINOR_VERSION == 19
	TArray<void*> Addresses;
	ArticyRefPropertyHandle->AccessRawData(Addresses);
	void* ArticyRefAddress = Addresses[0];
	ArticyRef = static_cast<FArticyRef*>(ArticyRefAddress);
#endif

	return ArticyRef;
}
