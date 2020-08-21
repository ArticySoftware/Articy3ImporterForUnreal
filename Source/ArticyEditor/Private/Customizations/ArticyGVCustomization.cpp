//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Customizations/ArticyGVCustomization.h"

#include "ArticyGlobalVariables.h"
#include "IDetailChildrenBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/SWidget.h"
#include "Modules/ModuleManager.h"
#include "IDetailPropertyRow.h"
#include "Delegates/Delegate.h"
#include "ArticyObject.h"
#include "ArticyRef.h"
#include "ClassViewerModule.h"
#include "UObject/ConstructorHelpers.h"
#include "ArticyEditorModule.h"
#include "Slate/GV/SArticyGlobalVariablesDebugger.h"

void FArticyGVCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	if (WeakObjects.Num() != 1) return;

	TArray<UObject*> Objects;
	CopyFromWeakArray(Objects, WeakObjects);

	UObject* Object = Objects[0];

	SizeData.RightColumnWidth = TAttribute<float>(this, &FArticyGVCustomization::OnGetRightColumnWidth);
	SizeData.LeftColumnWidth = TAttribute<float>(this, &FArticyGVCustomization::OnGetLeftColumnWidth);
	SizeData.OnWidthChanged = SSplitter::FOnSlotResized::CreateSP(this, &FArticyGVCustomization::OnSetColumnWidth);

	TMap<FString, TSharedRef<IPropertyHandle>> Properties;
	TArray<FName> CategoryNames;
	DetailBuilder.GetCategoryNames(CategoryNames);

	IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(TEXT("Default"));

	// retrieve properties and hide categories so we can customize it more properly
	// we have to retrieve the properties via categories because otherwise we can't get access to the properties of child classes
	for (FName CatName : CategoryNames)
	{
		IDetailCategoryBuilder& Builder = DetailBuilder.EditCategory(CatName);
		TArray<TSharedRef<IPropertyHandle>> Props;
		Builder.GetDefaultProperties(Props);

		DetailBuilder.HideCategory(CatName);
		for (TSharedRef<IPropertyHandle> Handle : Props)
		{
			Properties.Add(Handle->GetProperty()->GetNameCPP(), Handle);
			UE_LOG(LogArticyEditor, Warning, TEXT("Cat Props %s"), *Handle->GetProperty()->GetNameCPP());
		}
	}

	// retrieve the propertyhandles for the properties in the class (which are variablesets), and create widgets for them
	for(TFieldIterator<FObjectProperty> SetIt(Object->GetClass()); SetIt; ++SetIt)
	{
		UE_LOG(LogArticyEditor, Warning, TEXT("%s"), *SetIt->GetName());
		if(Properties.Contains(*SetIt->GetName()))
		{
			TSharedRef<IPropertyHandle> Handle = Properties[*SetIt->GetName()];
			UObject* VarSetObject = nullptr;
			Handle->GetValue(VarSetObject);

			UArticyBaseVariableSet* VarSet = Cast<UArticyBaseVariableSet>(VarSetObject);
			ensure(VarSet);
			
			TSharedRef<SArticyVariableSet> VarSetWidget = SNew(SArticyVariableSet, VarSet)
			.SizeData(&SizeData);
			
			FDetailWidgetRow& WidgetRow = CategoryBuilder.AddCustomRow(Handle->GetPropertyDisplayName());
			WidgetRow
			[
				VarSetWidget
			];
		}
	}
}

TSharedRef<IDetailCustomization> FArticyGVCustomization::MakeInstance()
{
	return MakeShareable(new FArticyGVCustomization());
}

TSharedRef<IDetailCustomization> FArticyGVCustomization2::MakeInstance()
{
	return MakeShareable(new FArticyGVCustomization2());
}

void FArticyGVCustomization2::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> WeakObjects;
	DetailBuilder.GetObjectsBeingCustomized(WeakObjects);

	if (WeakObjects.Num() != 1) return;

	TArray<UObject*> Objects;
	CopyFromWeakArray(Objects, WeakObjects);

	UObject* Object = Objects[0];

	SizeData.RightColumnWidth = TAttribute<float>(this, &FArticyGVCustomization2::OnGetRightColumnWidth);
	SizeData.LeftColumnWidth = TAttribute<float>(this, &FArticyGVCustomization2::OnGetLeftColumnWidth);
	SizeData.OnWidthChanged = SSplitter::FOnSlotResized::CreateSP(this, &FArticyGVCustomization2::OnSetColumnWidth);

	TMap<FString, TSharedRef<IPropertyHandle>> Properties;
	TArray<FName> CategoryNames;
	DetailBuilder.GetCategoryNames(CategoryNames);

	IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(TEXT("Default"));

	// retrieve properties and hide categories so we can customize it more properly
	// we have to retrieve the properties via categories because otherwise we can't get access to the properties of child classes
	for (FName CatName : CategoryNames)
	{
		IDetailCategoryBuilder& Builder = DetailBuilder.EditCategory(CatName);
		TArray<TSharedRef<IPropertyHandle>> Props;
		Builder.GetDefaultProperties(Props);

		DetailBuilder.HideCategory(CatName);
		for (TSharedRef<IPropertyHandle> Handle : Props)
		{
			Properties.Add(Handle->GetProperty()->GetNameCPP(), Handle);
			FProperty* Prop = Handle->GetProperty();
			FProperty* ParentProp = Handle->GetProperty()->GetOwnerProperty();
			
			UE_LOG(LogArticyEditor, Warning, TEXT("Cat Props %s"), *Prop->GetNameCPP());
			
		}
	}
}
