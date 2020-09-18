//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/Pins/SArticyRefPin.h"
#include "ArticyRef.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraph/EdGraphNode.h"
#include "Slate/SArticyIdProperty.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"


void SArticyRefPin::Construct(const FArguments& InArgs, UEdGraphPin* GraphPin)
{
	ArticyId = FArticyId();
	SGraphPin::Construct(SGraphPin::FArguments(), GraphPin);
}

TSharedRef<SWidget> SArticyRefPin::GetDefaultValueWidget()
{
	return SNew(SArticyIdProperty)
		.ArticyIdToDisplay(this, &SArticyRefPin::GetArticyId)
		.OnArticyIdChanged(this, &SArticyRefPin::OnArticyIdChanged)
		.Visibility(this, &SArticyRefPin::GetDefaultValueVisibility);
}

EVisibility SArticyRefPin::GetDefaultValueVisibility() const
{
	if (bOnlyShowDefaultValue)
	{
		return EVisibility::Visible;
	}
	
	const UEdGraphSchema* Schema = !GraphPinObj->IsPendingKill() ? GraphPinObj->GetSchema() : nullptr;
	if (Schema == nullptr || Schema->ShouldHidePinDefaultValue(GraphPinObj))
	{
		return EVisibility::Collapsed;
	}

	// If it's not connectable we want to always show it
	if (!GraphPinObj->bOrphanedPin && GraphPinObj->bNotConnectable)
	{
		return EVisibility::Visible;
	}

	// don't show on outputs
	if (GraphPinObj->Direction == EGPD_Output)
	{
		return EVisibility::Collapsed;
	}
	
	// show only when not connected
	return IsConnected() ? EVisibility::Collapsed : EVisibility::Visible;
}

FArticyId SArticyRefPin::GetArticyId() const
{
	FString IdString = GraphPinObj->GetDefaultAsString();
	FArticyId Id;

	if(!Id.InitFromString(IdString))
	{
		Id = FArticyId();
	}
	
	return Id;
}

void SArticyRefPin::OnArticyIdChanged(const FArticyId& NewArticyId)
{
	FString FormattedValueString;
	FArticyRef::StaticStruct()->ExportText(FormattedValueString, &ArticyId, nullptr, nullptr, (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_IncludeTransient), nullptr);

	ArticyId = NewArticyId;
	
	// remove the old ID string
	const int32 IdIndex = FormattedValueString.Find(FString(TEXT("Id=(")), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	const int32 EndOfIdIndex = FormattedValueString.Find(FString(TEXT(")")), ESearchCase::IgnoreCase, ESearchDir::FromStart, IdIndex);
	FormattedValueString.RemoveAt(IdIndex, EndOfIdIndex - IdIndex);

	// reconstruct the value string with the new ID
	const FString NewIdString = FString::Format(TEXT("Id=(Low={0}, High={1})"), { NewArticyId.Low, NewArticyId.High });
	FormattedValueString.InsertAt(IdIndex, *NewIdString);

	if(GraphPinObj->GetDefaultAsString() != FormattedValueString)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeArticyRefValue", "Change Articy Ref Value"));
		GraphPinObj->Modify();
		
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, FormattedValueString);

		if (OwnerNodePtr.IsValid())
		{
			//OwnerNodePtr.Pin()->UpdateGraphNode();
		}
	}
}
