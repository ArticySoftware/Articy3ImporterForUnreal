//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/SArticyRefProperty.h"
#include <Templates/SharedPointer.h>
#include <Kismet2/KismetEditorUtilities.h>
#include <Kismet2/SClassPickerDialog.h>
#include <ARFilter.h>
#include <IContentBrowserSingleton.h>
#include <ContentBrowserModule.h>
#include <Widgets/Input/SComboButton.h>
#include "ArticyObject.h"
#include "ArticyEditorModule.h"
#include "Slate/AssetPicker/SArticyObjectAssetPicker.h"
#include "Editor.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "Customizations/ArticyRefCustomization.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxDefs.h"

#define LOCTEXT_NAMESPACE "ArticyRefProperty"

void SArticyRefProperty::Construct(const FArguments& InArgs, IPropertyHandle* InArticyRefPropHandle)
{
	this->ClassRestriction = InArgs._ClassRestriction;

	ArticyRefPropertyHandle = InArticyRefPropHandle;

	FString CurrentRefValue;
	InArticyRefPropHandle->GetValueAsFormattedString(CurrentRefValue);
	CurrentObjectID = FArticyRefCustomization::GetIdFromValueString(CurrentRefValue);
	CachedArticyObject = UArticyObject::FindAsset(CurrentObjectID);
	
	Cursor = EMouseCursor::Hand;

	if(!this->ClassRestriction.IsBound()) 
	{
		UE_LOG(LogArticyEditor, Warning, TEXT("Tried constructing articy ref property without valid class restriction. Using ArticyObject instead"));
		this->ClassRestriction = UArticyObject::StaticClass();
	}

	ComboButton = SNew(SComboButton)
		.OnGetMenuContent(this, &SArticyRefProperty::CreateArticyObjectAssetPicker)
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(this, &SArticyRefProperty::OnGetArticyObjectDisplayName)
		];

	TileView = SNew(SArticyObjectTileView)
		.ObjectToDisplay(this, &SArticyRefProperty::GetCurrentObjectID)
		.ThumbnailSize(ArticyRefPropertyConstants::ThumbnailSize.X)
		.ThumbnailPadding(ArticyRefPropertyConstants::ThumbnailPadding.X);

	ExtraButtons = SNew(SHorizontalBox);

	UpdateWidget();
	
	this->ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0, 0, 2, 0)
		[
			SAssignNew(ThumbnailBorder, SBorder)
			.Padding(5.0f)
			.OnMouseDoubleClick(this, &SArticyRefProperty::OnAssetThumbnailDoubleClick)
			[
				SAssignNew(TileContainer, SBox)
				[
					TileView.ToSharedRef()
				]
			]
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(3, 5, 3, 0)
			[
				ComboButton.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Padding(3, 0, 3, 2)
			[
				ExtraButtons.ToSharedRef()
			]
		]
	];
}

void SArticyRefProperty::UpdateWidget()
{
	FString RefString;
	const FPropertyAccess::Result Result = ArticyRefPropertyHandle->GetValueAsFormattedString(RefString);
	// the actual update. This will be forwarded into the tile view and will cause an update
	CurrentObjectID = FArticyRefCustomization::GetIdFromValueString(RefString);
	CachedArticyObject = UArticyObject::FindAsset(CurrentObjectID);

	// update the customizations
	for (TSharedPtr<IArticyRefWidgetCustomization>& Customization : ActiveCustomizations)
	{
		Customization->UnregisterArticyRefWidgetCustomization();
	}
	
	FArticyRef* Ref = FArticyRefCustomization::RetrieveArticyRef(ArticyRefPropertyHandle);

	ActiveCustomizations.Reset();
	FArticyEditorModule::Get().GetCustomizationManager()->CreateArticyRefWidgetCustomizations(*Ref, ActiveCustomizations);

	FArticyRefWidgetCustomizationBuilder Builder(*Ref);
	for(TSharedPtr<IArticyRefWidgetCustomization>& Customization : ActiveCustomizations)
	{
		Customization->RegisterArticyRefWidgetCustomization(Builder);
	}
	
	ApplyArticyRefCustomizations(Builder.GetCustomizations());
}

void SArticyRefProperty::ApplyArticyRefCustomization(const FArticyRefWidgetCustomizationInfo& Customization)
{
	if(Customization.ExtraButtonExtender != nullptr)
	{
		ExtraButtonExtenders.Add(Customization.ExtraButtonExtender);
	}
}

void SArticyRefProperty::ApplyArticyRefCustomizations(const TArray<FArticyRefWidgetCustomizationInfo>& Customizations)
{
	ExtraButtonExtenders.Empty();
	ExtraButtons->ClearChildren();

	for(const FArticyRefWidgetCustomizationInfo& Info : Customizations)
	{
		ApplyArticyRefCustomization(Info);
	}

	TSharedPtr<FExtender> ExtraButtonExtender = FExtender::Combine(ExtraButtonExtenders);
	FToolBarBuilder Builder(nullptr, FMultiBoxCustomization(TEXT("ExtraButtons")), ExtraButtonExtender);

	// we need to begin a section so the extenders know where to apply themselves
	Builder.BeginSection(TEXT("Base"));
	Builder.EndSection();

	ExtraButtons->AddSlot()
	[
		Builder.MakeWidget()
	];
}

void SArticyRefProperty::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	FString RefString;
	const FPropertyAccess::Result Result = ArticyRefPropertyHandle->GetValueAsFormattedString(RefString);

	if(Result == FPropertyAccess::Success)
	{
		const FArticyId CurrentRefId = FArticyRefCustomization::GetIdFromValueString(RefString);
		if (CurrentRefId != CurrentObjectID || !CachedArticyObject.IsValid())
		{
			UpdateWidget();
		}
	}	
}

TSharedRef<SWidget> SArticyRefProperty::CreateArticyObjectAssetPicker()
{	
	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateRaw(this, &SArticyRefProperty::SetAsset);
	AssetPickerConfig.bFocusSearchBoxWhenOpened = true;
	AssetPickerConfig.Filter.ClassNames.Add(FName(*ClassRestriction.Get()->GetName()));

	return SNew(SArticyObjectAssetPicker).AssetPickerConfig(AssetPickerConfig);
}

FReply SArticyRefProperty::OnArticyButtonClicked() const
{
	UserInterfaceHelperFunctions::ShowObjectInArticy(UArticyObject::FindAsset(CurrentObjectID));
	return FReply::Handled();
}

void SArticyRefProperty::SetAsset(const FAssetData& AssetData) const
{
	// retrieve the newly selected articy object
	ComboButton->SetIsOpen(false);
	const UArticyObject* NewSelection  = Cast<UArticyObject>(AssetData.GetAsset());

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

FReply SArticyRefProperty::OnAssetThumbnailDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) const
{
	if(CachedArticyObject.IsValid())
	{
		GEditor->EditObject(CachedArticyObject.Get());
	}

	return FReply::Handled();
}

FText SArticyRefProperty::OnGetArticyObjectDisplayName() const
{
	const FString DisplayName = UserInterfaceHelperFunctions::GetDisplayName(CachedArticyObject.Get());
	return FText::FromString(DisplayName);
}

FArticyId SArticyRefProperty::GetCurrentObjectID() const
{
	return CurrentObjectID;
}

#undef LOCTEXT_NAMESPACE

