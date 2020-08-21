//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/GV/SArticyGlobalVariables.h"
#include "ArticyEditorStyle.h"
#include "ArticyPluginSettings.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

void SArticyVariableSet::Construct(const FArguments& Args, TWeakObjectPtr<UArticyBaseVariableSet> InVariableSet)
{
	VariableSet = InVariableSet;

	ensure(VariableSet.IsValid());
	
	SizeData = Args._SizeData;

	VariableContainer = SNew(SVerticalBox);

	NamespaceExpansion = SNew(SExpandableArea)
	.InitiallyCollapsed(Args._bInitiallyCollapsed)
	.BorderBackgroundColor(FLinearColor(0.7f, 0.7f, 0.7f))
	.HeaderContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(VariableSet->GetName()))
			.TextStyle(FArticyEditorStyle::Get(), TEXT("ArticyImporter.GlobalVariables.Namespace"))
		]
	]
	.BodyContent()
	[
		VariableContainer.ToSharedRef()
	];

	BuildVariableWidgets();

	ChildSlot
	[
		NamespaceExpansion.ToSharedRef()
	];
}

void SArticyVariableSet::SetExpanded(bool bInExpanded) const
{
	NamespaceExpansion->SetExpanded(bInExpanded);
}

bool SArticyVariableSet::IsExpanded() const
{
	return NamespaceExpansion->IsExpanded();
}

void SArticyVariableSet::BuildVariableWidgets()
{
	if (bVariableWidgetsBuilt)
	{
		return;
	}

	TArray<UArticyVariable*> SortedVars = VariableSet->Variables;
	SortedVars.Sort([](const UArticyVariable& LHS, const UArticyVariable& RHS)
	{
		return LHS.GetName().Compare(RHS.GetName(), ESearchCase::IgnoreCase) < 0 ? true : false;
	});
	
	for (UArticyVariable* Var : SortedVars)
	{
		TSharedRef<SSplitter> LocalSplitter = SNew(SSplitter);

		// left variable slot
		LocalSplitter->AddSlot()
		.Value(SizeData->LeftColumnWidth)
		.OnSlotResized(SizeData->OnWidthChanged)
		[
			SNew(STextBlock).Text_Lambda([Var]()
			{
				return FText::FromString(Var->GetName());
			})
		];

		// right variable slot
		SSplitter::FSlot& RightVariableSlot = LocalSplitter->AddSlot()
			.Value(SizeData->RightColumnWidth)
			.OnSlotResized(SizeData->OnWidthChanged);

		TSharedRef<SHorizontalBox> ConstrainBox = SNew(SHorizontalBox);
		SHorizontalBox::FSlot& InnerVarSlot = ConstrainBox->AddSlot().AutoWidth();

		RightVariableSlot
		[
			SNew(SBox)
			.MinDesiredWidth(150.f)
			.MaxDesiredWidth(300.f)
			[
				ConstrainBox
			]
		];

		if (Var->GetClass() == UArticyString::StaticClass())
		{
			UArticyString* StringVar = Cast<UArticyString>(Var);
			InnerVarSlot
			[
				SNew(SEditableTextBox)
				.MinDesiredWidth(30.f)
				.Text_Lambda([StringVar]()
				{
					return FText::FromString(StringVar->Get());
				})
				.OnTextCommitted_Lambda([StringVar](const FText& Text, ETextCommit::Type CommitType)
				{
					if(StringVar->Get().Equals(Text.ToString()))
					{
						return;
					}
					
					GEditor->BeginTransaction(FText::FromString(TEXT("ModifyArticyGlobalVariable")));
					StringVar->Modify();
					*StringVar = Text.ToString();
					GEditor->EndTransaction();
				})
			];
		}
		else if (Var->GetClass() == UArticyInt::StaticClass())
		{
			UArticyInt* IntVar = Cast<UArticyInt>(Var);
			InnerVarSlot
			[
				SNew(SNumericEntryBox<int32>)
				.AllowSpin(true)
				.MaxSliderValue(TOptional<int32>())
				.MinSliderValue(TOptional<int32>())
				.MinDesiredValueWidth(80.f)
				.OnBeginSliderMovement_Lambda([=]() { bSliderMoving = true; GEditor->BeginTransaction(FText::FromString(TEXT("Modify Global Var by Slider"))); })
				.OnEndSliderMovement_Lambda([=](int32 Value) { bSliderMoving = false; IntVar->Modify();  *IntVar = Value; GEditor->EndTransaction(); })
				.Value_Lambda([IntVar]()
				{
					return IntVar->Get();
				})
				.OnValueChanged(this, &SArticyVariableSet::OnValueChanged, IntVar)
				.OnValueCommitted_Lambda([=](int32 Value, ETextCommit::Type Type)
				{
					if (Value == IntVar->Get())
					{
						return;
					}

					GEditor->BeginTransaction(FText::FromString(TEXT("ModifyArticyGlobalVariable")));
					IntVar->Modify();
					*IntVar = Value;
					GEditor->EndTransaction();
				})
			];
		}
		else if (Var->GetClass() == UArticyBool::StaticClass())
		{
			UArticyBool* BoolVar = Cast<UArticyBool>(Var);
			InnerVarSlot
			[
				SNew(SCheckBox)
				.IsChecked_Lambda([BoolVar]()
				{
					return BoolVar->Get() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				})
				.OnCheckStateChanged_Lambda([BoolVar](const ECheckBoxState& State)
				{
					GEditor->BeginTransaction(FText::FromString(TEXT("ModifyArticyGlobalVariable")));
					BoolVar->Modify();
					*BoolVar = State == ECheckBoxState::Checked ? true : false;
					GEditor->EndTransaction();
				})
			];
		}

		VariableWidgetMapping.Add(Var, LocalSplitter);

		VariableContainer->AddSlot()
			.AutoHeight()
			.Padding(25.f, 5.f, 5.f, 5.f)
			[
				LocalSplitter
			];
	}

	bVariableWidgetsBuilt = true;
}

void SArticyVariableSet::UpdateVisibility(const UArticyVariable* Variable, EVisibility InVisibility)
{
	if(VariableWidgetMapping.Contains(Variable))
	{
		TWeakPtr<SWidget> VarWidget = VariableWidgetMapping[Variable];
		VarWidget.Pin()->SetVisibility(InVisibility);
	}
}

TArray<UArticyVariable*> SArticyVariableSet::GetVariables()
{
	return VariableSet->GetVariables();
}

void SArticyGlobalVariables::Construct(const FArguments& Args, TWeakObjectPtr<UArticyGlobalVariables> GV)
{
	VariableFilter = MakeShareable(new FFrontendFilter_ArticyVariable);
	
	FrontendFilters = MakeShareable(new FArticyVariableFilterCollectionType);
	FrontendFilters->OnChanged().AddSP(this, &SArticyGlobalVariables::OnFrontendFiltersChanged);
	GlobalVariables = GV;

	SizeData.RightColumnWidth = TAttribute<float>(this, &SArticyGlobalVariables::OnGetRightColumnWidth);
	SizeData.LeftColumnWidth = TAttribute<float>(this, &SArticyGlobalVariables::OnGetLeftColumnWidth);
	SizeData.OnWidthChanged = SSplitter::FOnSlotResized::CreateSP(this, &SArticyGlobalVariables::OnSetColumnWidth);

	bInitiallyCollapsed = Args._bInitiallyCollapsed;
	
	TSharedRef<SVerticalBox> ParentWidget = SNew(SVerticalBox);
	SetContainer = SNew(SVerticalBox);

	TSharedRef<SScrollBox> ScrollBox = SNew(SScrollBox).ScrollBarAlwaysVisible(true)
	+ SScrollBox::Slot()
	[
		SetContainer.ToSharedRef()
	];

	if(GlobalVariables.IsValid())
	{
		UpdateDisplayedGlobalVariables(GlobalVariables);
	}
	
	TSharedRef<SSearchBox> SearchBox = SNew(SSearchBox)
		.OnTextChanged(this, &SArticyGlobalVariables::OnSearchBoxChanged)
		.OnTextCommitted(this, &SArticyGlobalVariables::OnSearchBoxCommitted)
		.DelayChangeNotificationsWhileTyping(true);

	ParentWidget->AddSlot().AutoHeight()[SearchBox];
	ParentWidget->AddSlot().FillHeight(1.f)[ScrollBox];

	ChildSlot
	[
		ParentWidget
	];

	//FrontendFilters->Add(VariableFilter);
}

void SArticyGlobalVariables::UpdateDisplayedGlobalVariables(TWeakObjectPtr<UArticyGlobalVariables> InGV)
{
	VariableSetWidgets.Empty();
	SetContainer->ClearChildren();

	if(!InGV.IsValid())
	{
		return;
	}
	
	TArray<UArticyBaseVariableSet*> SortedSets = InGV->GetVariableSets();
	SortedSets.Sort([](const UArticyBaseVariableSet& LHS, const UArticyBaseVariableSet& RHS)
	{
		return LHS.GetName().Compare(RHS.GetName(), ESearchCase::IgnoreCase) < 0 ? true : false;
	});

	for (UArticyBaseVariableSet* Set : SortedSets)
	{
		TSharedRef<SArticyVariableSet> VarSetWidget = SNew(SArticyVariableSet, Set)
		.bInitiallyCollapsed(bInitiallyCollapsed)
		.SizeData(&SizeData);

		VariableSetWidgets.Add(VarSetWidget);

		SetContainer->AddSlot().AutoHeight()
		[
			VarSetWidget
		];
	}

	// retrigger the currently active filters
	FrontendFilters->OnChanged().Broadcast();
}

void SArticyGlobalVariables::OnSearchBoxChanged(const FText& InSearchText)
{
	SetSearchBoxText(InSearchText);
}

void SArticyGlobalVariables::OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo)
{
	SetSearchBoxText(InSearchText);
}

void SArticyGlobalVariables::SetSearchBoxText(const FText& InSearchText)
{
	// Update the filter text only if it has actually changed, including case sensitivity (as operators are case sensitive)
	if (!InSearchText.ToString().Equals(VariableFilter->GetRawFilterText().ToString(), ESearchCase::CaseSensitive))
	{
		VariableFilter->SetRawFilterText(InSearchText);

		// add or remove the filter depending on whether the search field has content or not
		if (InSearchText.IsEmpty() && FrontendFilters->Num() > 0)
		{
			FrontendFilters->Remove(VariableFilter);
			RestoreExpansionStates();
		}
		else if(!InSearchText.IsEmpty() && FrontendFilters->Num() == 0)
		{
			CacheExpansionStates();
			FrontendFilters->Add(VariableFilter);
		}
	}
}

void SArticyGlobalVariables::OnFrontendFiltersChanged()
{
	if(FrontendFilters->Num() > 0)
	{
		bShouldForceExpand = true;
	}
	else
	{
		bShouldForceExpand = false;
	}
	
	for(TSharedPtr<SArticyVariableSet> SetWidget : VariableSetWidgets)
	{
		uint32 NumVisible = 0;
		TArray<UArticyVariable*> Vars = SetWidget->GetVariables();

		for(UArticyVariable* Var : Vars)
		{
			const UArticyVariable* VarTmp = Var;
			if(TestAgainstFrontendFilters(Var))
			{
				SetWidget->UpdateVisibility(VarTmp, EVisibility::Visible);
				NumVisible++;
			}
			else
			{
				SetWidget->UpdateVisibility(VarTmp, EVisibility::Collapsed);
			}
		}

		if(NumVisible > 0)
		{
			if(bShouldForceExpand)
			{
				SetWidget->SetExpanded(true);
			}
			
			SetWidget->SetVisibility(EVisibility::Visible);
		}
		else
		{
			SetWidget->SetVisibility(EVisibility::Collapsed);
		}
	}
}

bool SArticyGlobalVariables::TestAgainstFrontendFilters(const UArticyVariable* Item) const
{
	if (FrontendFilters.IsValid() && !FrontendFilters->PassesAllFilters(Item))
	{
		return false;
	}

	return true;
}

void SArticyGlobalVariables::CacheExpansionStates()
{
	for(TSharedPtr<SArticyVariableSet>& SetWidget : VariableSetWidgets)
	{
		ExpansionCache.Add(SetWidget, SetWidget->IsExpanded());
	}
}

void SArticyGlobalVariables::RestoreExpansionStates()
{
	// restore the previous expansion state from the forced expansion
	for (auto It = ExpansionCache.CreateIterator(); It; ++It)
	{
		It.Key()->SetExpanded(It.Value());
	}
}