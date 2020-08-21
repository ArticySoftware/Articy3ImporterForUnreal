//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "ArticyGlobalVariables.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "Slate/ArticyFilterHelpers.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

/** ref: detailcategorygroupnode.cpp */
struct FGlobalVariablesSizeData
{
	TAttribute<float> LeftColumnWidth;
	TAttribute<float> RightColumnWidth;
	SSplitter::FOnSlotResized OnWidthChanged;

	void SetColumnWidth(float InWidth) { OnWidthChanged.ExecuteIfBound(InWidth); }
};

class SArticyVariableSet : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SArticyVariableSet) :
	_bInitiallyCollapsed(true),
	_SizeData(nullptr)
	{}

	SLATE_ARGUMENT(bool, bInitiallyCollapsed)
	SLATE_ARGUMENT(const FGlobalVariablesSizeData*, SizeData)
	
	SLATE_END_ARGS()

	void Construct(const FArguments& Args, TWeakObjectPtr<UArticyBaseVariableSet> VariableSet);
	void SetExpanded(bool bInExpanded) const;
	bool IsExpanded() const;
	void UpdateVisibility(const UArticyVariable* Variable, EVisibility InVisibility);
	TArray<UArticyVariable*> GetVariables();

private:
	void BuildVariableWidgets();

private:
	bool bVariableWidgetsBuilt = false;
	bool bSliderMoving = false;
	TWeakObjectPtr<UArticyBaseVariableSet> VariableSet;
	TMap<UArticyVariable*, TWeakPtr<SWidget>> VariableWidgetMapping;
	TSharedPtr<SVerticalBox> VariableContainer;
	TSharedPtr<SExpandableArea> NamespaceExpansion;

	const FGlobalVariablesSizeData* SizeData = nullptr;

	template<typename T, typename T2>
	void OnValueChanged(T Value, T2* Var);
};

template <typename T, typename T2>
void SArticyVariableSet::OnValueChanged(T Value, T2* Var)
{
	if(bSliderMoving)
	{
		bSliderMoving = true;
		
		if(Var->Get() == Value)
		{
			return;
		}
		Var->Modify();
		*Var = Value;
	}
}

class SArticyGlobalVariables : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SArticyGlobalVariables) :
	_bInitiallyCollapsed(true)
	{}
		SLATE_ARGUMENT(bool, bInitiallyCollapsed)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args, TWeakObjectPtr<UArticyGlobalVariables> GV);

	void UpdateDisplayedGlobalVariables(TWeakObjectPtr<UArticyGlobalVariables> InGV);
private:
	TWeakObjectPtr<UArticyGlobalVariables> GlobalVariables;

private:
	FGlobalVariablesSizeData SizeData;
	// ref: SDetailsView
	float ColumnWidth = 0.65f;
    bool bInitiallyCollapsed = true;
private:
	float OnGetLeftColumnWidth() const { return 1.0f - ColumnWidth; }
	float OnGetRightColumnWidth() const { return ColumnWidth; }
	void OnSetColumnWidth(float InWidth) { ColumnWidth = InWidth; }

private:
	void OnSearchBoxChanged(const FText& InSearchText);
	void OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo);
	void SetSearchBoxText(const FText& InSearchText);
	void OnFrontendFiltersChanged();
	bool TestAgainstFrontendFilters(const UArticyVariable* Item) const;

	void CacheExpansionStates();
	void RestoreExpansionStates();
	
private:
	TSharedPtr<SVerticalBox> SetContainer;
	//TSet<TWeakObjectPtr<UArticyVariable>> VisibleVariables;
	TArray<TSharedPtr<SArticyVariableSet>> VariableSetWidgets;
	/** Used to force expand all sets when a search has some results */
	bool bShouldForceExpand = false;
	/** Caches the current expansion state to restore it when the search terms are removed */
	TMap<TSharedPtr<SArticyVariableSet>, bool> ExpansionCache;
	/** The filter that checks against variable names */
	TSharedPtr<FFrontendFilter_ArticyVariable> VariableFilter;
	/** The collection of filters */
	TSharedPtr<FArticyVariableFilterCollectionType> FrontendFilters;
};
