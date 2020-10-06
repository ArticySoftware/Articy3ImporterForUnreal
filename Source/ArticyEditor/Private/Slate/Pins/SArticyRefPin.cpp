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
#include "Slate/SArticyRefProperty.h"

void SArticyRefPin::Construct(const FArguments& InArgs, UEdGraphPin* GraphPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), GraphPin);
}

TSharedRef<SWidget> SArticyRefPin::GetDefaultValueWidget()
{
	return SNew(SArticyRefProperty)
		.ArticyRefToDisplay(this, &SArticyRefPin::GetArticyRef)
		.OnArticyRefChanged(this, &SArticyRefPin::OnArticyRefChanged)
		.bIsReadOnly(true)
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

FArticyRef SArticyRefPin::GetArticyRef() const
{
	FString RefString = GraphPinObj->GetDefaultAsString();
	FArticyRef Ref;

	if(!Ref.InitFromString(RefString))
	{
		Ref = FArticyRef();
	}
	
	return Ref;
}

void SArticyRefPin::OnArticyRefChanged(const FArticyRef& NewArticyRef)
{
	FString FormattedValueString;
	FArticyRef::StaticStruct()->ExportText(FormattedValueString, &ArticyRef, nullptr, nullptr, (PPF_Copy), nullptr);

	ArticyRef = NewArticyRef;

	if(GraphPinObj->GetDefaultAsString() != FormattedValueString)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeArticyRefValue", "Change Articy Ref Value"));
		GraphPinObj->Modify();
		
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, ArticyRef.ToString());

		if (OwnerNodePtr.IsValid())
		{
			//OwnerNodePtr.Pin()->UpdateGraphNode();
		}
	}
}
