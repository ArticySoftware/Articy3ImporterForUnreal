//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/Pins/SArticyIdPin.h"
#include "ArticyRef.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraph/EdGraphNode.h"
#include "Slate/SArticyIdProperty.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Slate/SArticyRefProperty.h"

void SArticyIdPin::Construct(const FArguments& InArgs, UEdGraphPin* GraphPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), GraphPin);
}

TSharedRef<SWidget> SArticyIdPin::GetDefaultValueWidget()
{
	return SNew(SArticyIdProperty)
		.ArticyIdToDisplay(this, &SArticyIdPin::GetArticyId)
		.OnArticyIdChanged(this, &SArticyIdPin::OnArticyIdChanged)
		.Visibility(this, &SArticyIdPin::GetDefaultValueVisibility);
}

EVisibility SArticyIdPin::GetDefaultValueVisibility() const
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

FArticyId SArticyIdPin::GetArticyId() const
{
	FString IdString = GraphPinObj->GetDefaultAsString();
	FArticyId Id;

	if(!Id.InitFromString(IdString))
	{
		Id = FArticyId();
	}
	
	return Id;
}

void SArticyIdPin::OnArticyIdChanged(const FArticyId& NewArticyId)
{
	FString FormattedValueString;
	FArticyRef::StaticStruct()->ExportText(FormattedValueString, &NewArticyId, nullptr, nullptr, (PPF_Copy), nullptr);

	ArticyId = NewArticyId;

	if(GraphPinObj->GetDefaultAsString() != FormattedValueString)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeArticyIdValue", "Change Articy Id Value"));
		GraphPinObj->Modify();
		
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, NewArticyId.ToString());

		if (OwnerNodePtr.IsValid())
		{
			//OwnerNodePtr.Pin()->UpdateGraphNode();
		}
	}
}
