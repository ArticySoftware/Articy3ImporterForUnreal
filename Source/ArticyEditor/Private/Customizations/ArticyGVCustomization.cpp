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
	UArticyGlobalVariables* GV = Cast<UArticyGlobalVariables>(Object);
	
	SizeData.RightColumnWidth = TAttribute<float>(this, &FArticyGVCustomization::OnGetRightColumnWidth);
	SizeData.LeftColumnWidth = TAttribute<float>(this, &FArticyGVCustomization::OnGetLeftColumnWidth);
	SizeData.OnWidthChanged = SSplitter::FOnSlotResized::CreateSP(this, &FArticyGVCustomization::OnSetColumnWidth);

	TMap<FString, TSharedRef<IPropertyHandle>> Properties;
	TArray<FName> CategoryNames;
	DetailBuilder.GetCategoryNames(CategoryNames);

	IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory(TEXT("Default"));

	// hide all normal categories as we'll replace them with a custom UI
	for (FName CatName : CategoryNames)
	{
		DetailBuilder.HideCategory(CatName);
	}

	FDetailWidgetRow& Row = CategoryBuilder.AddCustomRow(FText::FromString(TEXT("Articy")));
	Row.WholeRowWidget
	[
		SNew(SArticyGlobalVariables, GV).bInitiallyCollapsed(true)
	];

	//// retrieve the propertyhandles for the properties in the class (which are variablesets), and create widgets for them
	//for(TFieldIterator<FObjectProperty> SetIt(Object->GetClass()); SetIt; ++SetIt)
	//{
	//	UE_LOG(LogArticyEditor, Warning, TEXT("%s"), *SetIt->GetName());
	//	if(Properties.Contains(*SetIt->GetName()))
	//	{
	//		TSharedRef<IPropertyHandle> Handle = Properties[*SetIt->GetName()];
	//		UObject* VarSetObject = nullptr;
	//		Handle->GetValue(VarSetObject);

	//		UArticyBaseVariableSet* VarSet = Cast<UArticyBaseVariableSet>(VarSetObject);
	//		ensure(VarSet);
	//		
	//		TSharedRef<SArticyVariableSet> VarSetWidget = SNew(SArticyVariableSet, VarSet)
	//		.SizeData(&SizeData);
	//		
	//		FDetailWidgetRow& WidgetRow = CategoryBuilder.AddCustomRow(Handle->GetPropertyDisplayName());
	//		WidgetRow
	//		[
	//			VarSetWidget
	//		];
	//	}
	//}
}

TSharedRef<IDetailCustomization> FArticyGVCustomization::MakeInstance()
{
	return MakeShareable(new FArticyGVCustomization());
}
