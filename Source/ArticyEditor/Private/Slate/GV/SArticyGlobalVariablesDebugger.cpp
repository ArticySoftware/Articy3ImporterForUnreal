//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Slate/GV/SArticyGlobalVariablesDebugger.h"
#include "Slate/GV/SArticyGlobalVariables.h"
#include "ArticyPluginSettings.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "ArticyGlobalVarSelection"

void SArticyGlobalVariablesRuntimeDebugger::Construct(const FArguments& Args)
{
	// the runtime switcher will display a 'disabled' message if no game is running
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	RuntimeSwitcher = SNew(SWidgetSwitcher).WidgetIndex_Lambda([=, this]()
#else
	RuntimeSwitcher = SNew(SWidgetSwitcher).WidgetIndex_Lambda([=]()
#endif
	{
		return CurrentGlobalVariables.IsValid() ? 1 : 0;
	});
	
	GlobalVariablesWidget = SNew(SArticyGlobalVariables, CurrentGlobalVariables).bInitiallyCollapsed(Args._bInitiallyCollapsed);

	FMenuBuilder Builder(true, nullptr);

	TAttribute<FText> LabelAttribute;
	LabelAttribute.BindRaw(this, &SArticyGlobalVariablesRuntimeDebugger::GetCurrentGVLabel);
	Builder.BeginSection(TEXT("Select"));
		Builder.AddSubMenu(
		LabelAttribute,
		LOCTEXT("ChooseGVSubMenuToolTip", "Choose the runtime Global Variables instance to display."),
		FNewMenuDelegate::CreateSP(this, &SArticyGlobalVariablesRuntimeDebugger::BuildGVPickerContent),
		true		
		);
	Builder.EndSection();

	RuntimeSwitcher
	->AddSlot(0)
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	[
		SNew(STextBlock)
		.Text(FText::FromString(TEXT("Please start the game to access the runtime Global Variables debugger")))
	];

	RuntimeSwitcher
	->AddSlot(1)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Left)
		.Padding(5.f)
		[
			Builder.MakeWidget()
		]
		+ SVerticalBox::Slot()
		// if this was autoheight the scrollbar would stop working properly
		.FillHeight(1.f)
		[
			GlobalVariablesWidget.ToSharedRef()
		]	
	];
	ChildSlot
	[
		RuntimeSwitcher.ToSharedRef()
	];
}

void SArticyGlobalVariablesRuntimeDebugger::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime,	const float InDeltaTime)
{
	if(!CurrentGlobalVariables.IsValid() && GEditor->GetPIEWorldContext())
	{
		UpdateGVInstance(UArticyGlobalVariables::GetDefault(GEditor->GetPIEWorldContext()->World()));
	}
	else if(CurrentGlobalVariables.IsValid() && !GEditor->GetPIEWorldContext())
	{
		UpdateGVInstance(nullptr);
	}
}

void SArticyGlobalVariablesRuntimeDebugger::UpdateGVInstance(TWeakObjectPtr<UArticyGlobalVariables> InGVs)
{
	CurrentGlobalVariables = InGVs;
	GlobalVariablesWidget->UpdateDisplayedGlobalVariables(CurrentGlobalVariables);
}

void SArticyGlobalVariablesRuntimeDebugger::BuildGVPickerContent(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("GlobalVariables", LOCTEXT("GVHeading", "Global Variables"));
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			UWorld* World = Context.World();
			if (World && (World->IsGameWorld()))
			{
				UArticyGlobalVariables* GV = UArticyGlobalVariables::GetDefault(World);
				MenuBuilder.AddMenuEntry(
					FText::FromString(GV->GetName()),
					LOCTEXT("ChooseGVToolTip", "Display available GV instances."),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateSP(this, &SArticyGlobalVariablesRuntimeDebugger::OnSelectGVs, MakeWeakObjectPtr(GV)),
						FCanExecuteAction(),
						FIsActionChecked::CreateSP(this, &SArticyGlobalVariablesRuntimeDebugger::IsGVChecked, MakeWeakObjectPtr(GV))
					),
					NAME_None,
					EUserInterfaceActionType::RadioButton
				);

				// if we want to keep our global variables, we'll only have a single clone, so break after the first one
				if (GetDefault<UArticyPluginSettings>()->bKeepGlobalVariablesBetweenWorlds)
				{
					break;
				}

			}
		}
		
	}
	MenuBuilder.EndSection();
}

void SArticyGlobalVariablesRuntimeDebugger::OnSelectGVs(TWeakObjectPtr<UArticyGlobalVariables> InVars)
{
	CurrentGlobalVariables = InVars;
}

bool SArticyGlobalVariablesRuntimeDebugger::IsGVChecked(TWeakObjectPtr<UArticyGlobalVariables> InVars) const
{
	return CurrentGlobalVariables == InVars;
}

FText SArticyGlobalVariablesRuntimeDebugger::GetCurrentGVLabel() const
{
	FString OutValue = TEXT("Current GV: ");

	if (CurrentGlobalVariables.IsValid())
	{
		return FText::FromString(OutValue.Append(CurrentGlobalVariables->GetName()));
	}

	return FText::FromString(OutValue.Append(TEXT("None")));
}

#undef LOCTEXT_NAMESPACE
