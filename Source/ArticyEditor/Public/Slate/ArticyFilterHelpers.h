//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "ArticyGlobalVariables.h"
#include "FrontendFilterBase.h"
#include "Misc/TextFilterExpressionEvaluator.h"
#include "ArticyObject.h"

#define LOCTEXT_NAMESPACE "ArticyFilterHelpers"

typedef const FAssetData& FArticyObjectFilterType;
typedef TFilterCollection<FArticyObjectFilterType> FArticyObjectFilterCollectionType;

typedef const UArticyVariable*& FArticyVariableFilterType;
typedef TFilterCollection<const UArticyVariable*&> FArticyVariableFilterCollectionType;

/** A filter for testing articy objects for various traits such as display name, speaker name, text etc. */
class FFrontendFilter_ArticyObject : public IFilter<FArticyObjectFilterType>
{
public:
	FFrontendFilter_ArticyObject();
	~FFrontendFilter_ArticyObject();

	// FFrontendFilter implementation
	virtual FString GetName() const { return TEXT("ArticyObjectFilter"); }
	virtual FText GetDisplayName() const { return LOCTEXT("FrontendFilter_ArticyObjectFilter", "Articy Object Filter"); }
	virtual FText GetToolTipText() const { return LOCTEXT("FrontendFilter_ArticyObjectTooltip", "Show only articy objects that match the criteria"); }

	// IFilter implementation
	virtual bool PassesFilter(FArticyObjectFilterType InItem) const override;
	
	DECLARE_DERIVED_EVENT(FFrontendFilter_ArticyObject, IFilter<FArticyObjectFilterType>::FChangedEvent, FChangedEvent);
	virtual FChangedEvent& OnChanged() override { return ChangedEvent; }

	FChangedEvent ChangedEvent;
protected:
	void BroadcastChangedEvent() const { ChangedEvent.Broadcast(); }
public:
	/** Returns the unsanitized and unsplit filter terms */
	FText GetRawFilterText() const;

	/** Set the Text to be used as the Filter's restrictions */
	void SetRawFilterText(const FText& InFilterText);

	/** Get the last error returned from lexing or compiling the current filter text */
	FText GetFilterErrorText() const;

	/** If bIncludeClassName is true, the text filter will include an asset's class name in the search */
	void SetIncludeClassName(const bool InIncludeClassName);

private:

	/** Transient context data, used when calling PassesFilter. Kept around to minimize re-allocations between multiple calls to PassesFilter */
	TSharedRef<class FFrontendFilter_ArticyObjectFilterExpressionContext> TextFilterExpressionContext;

	/** Expression evaluator that can be used to perform complex text filter queries */
	FTextFilterExpressionEvaluator TextFilterExpressionEvaluator;
};

class FArticyClassRestrictionFilter : public IFilter<FArticyObjectFilterType>
{
public:
	FArticyClassRestrictionFilter(TSubclassOf<UArticyObject> AllowedClass, bool bExactClass);

	void UpdateFilteredClass(TSubclassOf<UArticyObject> NewAllowedClass) {	AllowedClass = NewAllowedClass;	OnChanged().Broadcast(); }
	void UpdateExactClass(bool bNewExactClass) { bExactClass = bNewExactClass; OnChanged().Broadcast(); }
	// IFilter implementation
	virtual bool PassesFilter(FArticyObjectFilterType InItem) const override;
	
	// IFilter implementation
	DECLARE_DERIVED_EVENT(FArticyClassRestrictionFilter, IFilter<FArticyObjectFilterType>::FChangedEvent, FChangedEvent);
	virtual FChangedEvent& OnChanged() override { return ChangedEvent; }

	FChangedEvent ChangedEvent;
private:
	TSubclassOf<UArticyObject> AllowedClass = UArticyObject::StaticClass();
	bool bExactClass = false;
};

class FFrontendFilter_ArticyVariable : public IFilter<FArticyVariableFilterType>
{
public:
	FFrontendFilter_ArticyVariable();
	~FFrontendFilter_ArticyVariable();

	// FFrontendFilter implementation
	virtual FString GetName() const { return TEXT("ArticyVariableFilter"); }
	virtual FText GetDisplayName() const { return LOCTEXT("FrontendFilter_ArticyVariable", "Articy Variable Filter"); }
	virtual FText GetToolTipText() const { return LOCTEXT("FrontendFilter_ArticyVariableTooltip", "Show only articy variables that match the criteria"); }

	// IFilter implementation
	virtual bool PassesFilter(FArticyVariableFilterType InItem) const override;

public:
	/** Returns the unsanitized and unsplit filter terms */
	FText GetRawFilterText() const;

	/** Set the Text to be used as the Filter's restrictions */
	void SetRawFilterText(const FText& InFilterText);

	/** Get the last error returned from lexing or compiling the current filter text */
	FText GetFilterErrorText() const;

private:
	/** We only need one expression context for all comparisons, so reuse it */
	TSharedRef<class FFrontendFilter_ArticyGVFilterExpressionContext> TextFilterExpressionContext;
	/** Expression evaluator to test against text expressions */
	FTextFilterExpressionEvaluator TextFilterExpressionEvaluator;

	DECLARE_DERIVED_EVENT(FFrontendFilter, IFilter<FArticyVariableFilterType>::FChangedEvent, FChangedEvent);
	virtual FChangedEvent& OnChanged() override { return ChangedEvent; }

private:
	FChangedEvent ChangedEvent;

};

#undef LOCTEXT_NAMESPACE