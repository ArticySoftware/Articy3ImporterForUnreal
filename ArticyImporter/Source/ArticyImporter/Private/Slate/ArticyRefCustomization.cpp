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
#include "ClassViewerModule.h"
#include "EditorStyleSet.h"
#include "ArticyImporterStyle.h"
#include <Private/SlateEditorStyle.h>



TSharedRef<IPropertyTypeCustomization> FArticyRefCustomization::MakeInstance()
{
	return MakeShareable(new FArticyRefCustomization());
}

void FArticyRefCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ArticyRefPropertyHandle = PropertyHandle;

	// update the reference upon selecting the ref; this only serves cosmetic purposes. The underlying Id will not be changed
	FArticyRef* ArticyRef = RetrieveArticyRef();

	UArticyObject* SelectedObject = UArticyObject::FindAsset(ArticyRef->GetId());
	// set the selected class to the currently selected object because the class selection widget has no actual property as a basis to serialize and keep its state
	if (SelectedObject)
	{
		ClassRestriction = SelectedObject->UObject::GetClass();
	}

	if (ClassRestriction == nullptr)
	{
		ClassRestriction = UArticyObject::StaticClass();
	}
	ArticyRefSelection = SNew(SArticyRefSelection, ArticyRef, CustomizationUtils)
		.ClassRestriction(this, &FArticyRefCustomization::GetClassRestriction);

	HeaderRow.NameContent()
	[
		ArticyRefPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(150)
	[
		ArticyRefSelection.ToSharedRef()
	];
}

void FArticyRefCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	FClassViewerInitializationOptions config;
	config.DisplayMode = EClassViewerDisplayMode::TreeView;
	config.ClassFilter = MakeShareable(new FArticyRefClassFilter);

	ClassViewer = StaticCastSharedRef<SClassViewer>(FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(config, FOnClassPicked::CreateSP(this, &FArticyRefCustomization::OnClassPicked)));
	ClassRestrictionButton =
		SNew(SComboButton)
		.OnGetMenuContent(this, &FArticyRefCustomization::CreateClassPicker)
		.ContentPadding(2.f)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &FArticyRefCustomization::GetChosenClassName)
			.Font(FArticyImporterStyle::Get().GetFontStyle(TEXT("DialogueEditor.GraphNode.Text.Property")))
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
				.BorderImage(FSlateEditorStyle::GetBrush(TEXT("PropertyWindow.NoOverlayColor")))
				.Padding(FMargin(0.0f, 2.0f))
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Class Restriction"))
					.Font(FArticyImporterStyle::Get().GetFontStyle(TEXT("DialogueEditor.GraphNode.Text.Property")))
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
		ChildBuilder.AddProperty(ChildHandle);
	}
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
	ClassRestrictionButton->SetIsOpen(false);
}

TSharedRef<SWidget> FArticyRefCustomization::CreateClassPicker() const
{
	return ClassViewer.ToSharedRef();
}
