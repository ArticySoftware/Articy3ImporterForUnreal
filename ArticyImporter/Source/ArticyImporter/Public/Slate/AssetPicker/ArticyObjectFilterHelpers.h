// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <FrontendFilterBase.h>
#include <TextFilterExpressionEvaluator.h>
#include "ArticyObject.h"

#define LOCTEXT_NAMESPACE "ArticyObjectSearchBoxHelpers"

/** A filter for testing articy objects for various traits such as display name, speaker name, text etc. */
class FFrontendFilter_ArticyObject : public FFrontendFilter
{
public:
	FFrontendFilter_ArticyObject();
	~FFrontendFilter_ArticyObject();

	// FFrontendFilter implementation
	virtual FString GetName() const override { return TEXT("ArticyObjectFilter"); }
	virtual FText GetDisplayName() const override { return LOCTEXT("FrontendFilter_ArticyObjectFilter", "Articy Object Filter"); }
	virtual FText GetToolTipText() const override { return LOCTEXT("FrontendFilter_ArticyObjectTooltip", "Show only articy objects that match the criteria"); }

	// IFilter implementation
	virtual bool PassesFilter(FAssetFilterType InItem) const override;

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

class FArticyClassRestrictionFilter : public IFilter<FAssetFilterType>
{
public:
	FArticyClassRestrictionFilter();

	// IFilter implementation
	virtual bool PassesFilter(FAssetFilterType InItem) const override;

	TSubclassOf<UArticyObject> AllowedClass;

	// IFilter implementation
	DECLARE_DERIVED_EVENT(FArticyClassRestrictionFilter, IFilter<FAssetFilterType>::FChangedEvent, FChangedEvent);
	virtual FChangedEvent& OnChanged() override { return ChangedEvent; }

	FChangedEvent ChangedEvent;
};
#undef LOCTEXT_NAMESPACE