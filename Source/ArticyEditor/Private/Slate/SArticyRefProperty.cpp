//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/SArticyRefProperty.h"
#include <Templates/SharedPointer.h>
#include <Kismet2/KismetEditorUtilities.h>
#include <Kismet2/SClassPickerDialog.h>
#include "ArticyObject.h"
#include "ArticyEditorModule.h"
#include "ArticyEditorStyle.h"
#include "Slate/AssetPicker/SArticyObjectAssetPicker.h"
#include "Slate/UserInterfaceHelperFunctions.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SSpacer.h"

#define LOCTEXT_NAMESPACE "ArticyRefProperty"

void SArticyRefProperty::Construct(const FArguments& InArgs)
{
	if(ArticyRefToDisplay.IsBound())
	{
		ensureMsgf(InArgs._OnArticyRefChanged.IsBound(), TEXT("Since the shown object is given externally per event, the handler also needs to handle updates."));
	}

	this->ArticyRefToDisplay = InArgs._ArticyRefToDisplay;
	this->OnArticyRefChanged = InArgs._OnArticyRefChanged;
	this->TopLevelClassRestriction = InArgs._TopLevelClassRestriction;
	this->bExactClass = InArgs._bExactClass;
	this->bExactClassEditable = InArgs._bExactClassEditable;
	this->bIsReadOnly = InArgs._bIsReadOnly;
	
	SetCursor(EMouseCursor::Hand);
	
	CreateInternalWidgets();

	UpdateWidget();

	this->ChildSlot
	[
		ArticyIdProperty.ToSharedRef()
	];
}

void SArticyRefProperty::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	const FArticyRef& CurrentRef = ArticyRefToDisplay.IsBound() || ArticyRefToDisplay.IsSet() ? ArticyRefToDisplay.Get() : FArticyRef();
	if (CurrentRef != CachedArticyRef || (!CurrentRef.GetId().IsNull() && !CachedArticyObject.IsValid()))
	{
		Update(CurrentRef);
	}
}

void SArticyRefProperty::CreateInternalWidgets()
{
	FUIAction CopyAction;
	FUIAction PasteAction;

	CopyAction.ExecuteAction = FExecuteAction::CreateSP(this, &SArticyRefProperty::OnCopyProperty);
	PasteAction.CanExecuteAction = FCanExecuteAction::CreateSP(this, &SArticyRefProperty::CanPasteProperty);
	PasteAction.ExecuteAction = FExecuteAction::CreateSP(this, &SArticyRefProperty::OnPasteProperty);

	ArticyIdExtender = MakeShared<FExtender>();
	ArticyIdExtender->AddToolBarExtension("Base", EExtensionHook::After, nullptr, FToolBarExtensionDelegate::CreateSP(this, &SArticyRefProperty::CreateAdditionalRefWidgets));
	
	FOnArticyIdChanged OnArticyIdChanged = FOnArticyIdChanged::CreateSP(this, &SArticyRefProperty::OnArticyIdChanged);
	TAttribute<FArticyId> ArticyIdToDisplay;
	ArticyIdToDisplay.BindRaw(this, &SArticyRefProperty::GetArticyIdToDisplay);

	ArticyIdProperty = SNew(SArticyIdProperty)
		.ArticyIdToDisplay(ArticyIdToDisplay)
		.OnArticyIdChanged(OnArticyIdChanged)
		.TopLevelClassRestriction(TopLevelClassRestriction)
		.HighExtender(ArticyIdExtender)
		.bExactClass(bExactClass)
		.bExactClassEditable(bExactClassEditable)
		.bIsReadOnly(bIsReadOnly)
		.CopyAction(CopyAction)
		.PasteAction(PasteAction);
}


void SArticyRefProperty::OnArticyIdChanged(const FArticyId& ArticyId) 
{
	// kind of redundant
	CachedArticyRef.SetId(ArticyId);
	OnArticyRefChanged.ExecuteIfBound(CachedArticyRef);
}

FArticyId SArticyRefProperty::GetArticyIdToDisplay() const
{
	return ArticyRefToDisplay.Get().GetId();
}

void SArticyRefProperty::CreateAdditionalRefWidgets(FToolBarBuilder& Builder)
{
	TSharedRef<SHorizontalBox> AdditionalWidgetBox = SNew(SHorizontalBox)
#if __cplusplus >= 202002L
	.IsEnabled_Lambda([=, this]()
#else
	.IsEnabled_Lambda([=]()
#endif
	{
		return !bIsReadOnly.Get();
	})
	+ SHorizontalBox::Slot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.Padding(1.f, 3.f)
	.AutoWidth()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Base"))
		.TextStyle(&FArticyEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArticyImporter.SmallTextBlock"))
		.ToolTipText(FText::FromString("Should this ArticyRef reference the base object or a clone?"))
	]
	+ SHorizontalBox::Slot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.Padding(1.f, 3.f)
	.AutoWidth()
	[
		SNew(SCheckBox)
#if __cplusplus >= 202002L
		.IsChecked_Lambda([=, this]()
#else
		.IsChecked_Lambda([=]()
#endif
		{
			return CachedArticyRef.bReferenceBaseObject ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
		})
#if __cplusplus >= 202002L
		.OnCheckStateChanged_Lambda([=, this](ECheckBoxState NewState)
#else
		.OnCheckStateChanged_Lambda([=](ECheckBoxState NewState)
#endif
		{
			CachedArticyRef.bReferenceBaseObject = NewState == ECheckBoxState::Checked;
			OnArticyRefChanged.ExecuteIfBound(CachedArticyRef);
		})
	]
	+ SHorizontalBox::Slot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.FillWidth(1.f)
	[
		SNew(SSpacer)
	]
	+ SHorizontalBox::Slot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.Padding(1.f, 3.f)
	.AutoWidth()
	[
		SNew(STextBlock)
		.Text(FText::FromString("Clone"))
		.TextStyle(&FArticyEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArticyImporter.SmallTextBlock"))
		.ToolTipText(FText::FromString("Which clone Id should be referenced?"))
#if __cplusplus >= 202002L
		.Visibility_Lambda([=, this]()
#else
		.Visibility_Lambda([=]()
#endif
		{
			return CachedArticyRef.bReferenceBaseObject ? EVisibility::Collapsed : EVisibility::Visible;
		})
	]
	+ SHorizontalBox::Slot()
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.Padding(1.f, 0.f)
	.AutoWidth()
	[
		SNew(SNumericEntryBox<int32>)
		.MinDesiredValueWidth(15.f)
#if __cplusplus >= 202002L
		.Value_Lambda([=, this]()
#else
		.Value_Lambda([=]()
#endif
		{
			return CachedArticyRef.CloneId;
		})
#if __cplusplus >= 202002L
		.OnValueChanged_Lambda([=, this](int32 NewValue)
#else
		.OnValueChanged_Lambda([=](int32 NewValue)
#endif
		{
			CachedArticyRef.CloneId = NewValue;
			OnArticyRefChanged.ExecuteIfBound(CachedArticyRef);
		})
#if __cplusplus >= 202002L
		.Visibility_Lambda([=, this]()
#else
		.Visibility_Lambda([=]()
#endif
		{
			return CachedArticyRef.bReferenceBaseObject ? EVisibility::Collapsed : EVisibility::Visible;
		})
	];
	
	Builder.AddWidget(AdditionalWidgetBox);
}

void SArticyRefProperty::Update(const FArticyRef& NewRef)
{
	CachedArticyRef = NewRef;
	CachedArticyObject = !CachedArticyRef.GetId().IsNull() ? UArticyObject::FindAsset(CachedArticyRef.GetId()) : nullptr;
	
	UpdateWidget();
}

void SArticyRefProperty::UpdateWidget()
{
	// not yet done
}

void SArticyRefProperty::OnCopyProperty() const
{
	FString ValueString = CachedArticyRef.ToString();
	FPlatformApplicationMisc::ClipboardCopy(*ValueString);
}

void SArticyRefProperty::OnPasteProperty()
{
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	// copies over all values, then initializes with the string content. This makes both Ids and Refs pastable without deleting data
	FArticyRef NewRef = CachedArticyRef;
	const FArticyRef::EStringInitResult Success = NewRef.InitFromString(ClipboardContent);
	if (ensureMsgf(Success != FArticyRef::NoneSet, TEXT("String was garbage, therefore Ref was not properly updated")))
	{
		OnArticyRefChanged.ExecuteIfBound(NewRef);
		Update(NewRef);
	}
}

bool SArticyRefProperty::CanPasteProperty() const
{
	if (bIsReadOnly.Get())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	if (ClipboardContent.IsEmpty() || !ClipboardContent.Contains("Low=") || !ClipboardContent.Contains("High="))
	{
		return false;
	}

	FArticyId CandidateId;
	if(CandidateId.InitFromString(ClipboardContent))
	{
		UArticyObject* Object = UArticyObject::FindAsset(CandidateId);
		if(!Object)
		{
			return false;
		}

		return Object->UObject::GetClass()->IsChildOf(TopLevelClassRestriction.Get());
	}
	
	return false;
}
#undef LOCTEXT_NAMESPACE
