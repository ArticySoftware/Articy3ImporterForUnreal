//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "ArticyGlobalVariables.h"
#include "Slate/GV/SArticyGlobalVariables.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

class SArticyGlobalVariablesRuntimeDebugger : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SArticyGlobalVariablesRuntimeDebugger) :
		_bInitiallyCollapsed(true)
	{}
		SLATE_ARGUMENT(bool, bInitiallyCollapsed)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	
private:
	void UpdateGVInstance(TWeakObjectPtr<UArticyGlobalVariables> InGVs);
	void BuildGVPickerContent(FMenuBuilder& Builder);
	void OnSelectGVs(TWeakObjectPtr<UArticyGlobalVariables> InVars);
	bool IsGVChecked(TWeakObjectPtr<UArticyGlobalVariables> InVars) const;
	FText GetCurrentGVLabel() const;

private:
	TWeakObjectPtr<UArticyGlobalVariables> CurrentGlobalVariables;
	TSharedPtr<SArticyGlobalVariables> GlobalVariablesWidget;
	TSharedPtr<SWidget> WorldSelector;
	TSharedPtr<SWidgetSwitcher> RuntimeSwitcher;

	bool bIsActive = false;
	/** The handles for the PIE events */
	TArray<FDelegateHandle> RuntimeSwitchHandles;
};

#undef LOCTEXT_NAMESPACE