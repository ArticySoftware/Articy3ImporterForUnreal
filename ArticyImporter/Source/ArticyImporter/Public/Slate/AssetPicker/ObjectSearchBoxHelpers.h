// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <FrontendFilterBase.h>
#include <TextFilterExpressionEvaluator.h>
#include "ArticyObject.h"

#define LOCTEXT_NAMESPACE "DialogueEntityPicker"

/** A filter for text search */
class FFrontendFilter_DialogueEntity : public FFrontendFilter
{
public:
	FFrontendFilter_DialogueEntity();
	~FFrontendFilter_DialogueEntity();

	// FFrontendFilter implementation
	virtual FString GetName() const override { return TEXT("TextFilter"); }
	virtual FText GetDisplayName() const override { return LOCTEXT("FrontendFilter_Text", "Text"); }
	virtual FText GetToolTipText() const override { return LOCTEXT("FrontendFilter_TextTooltip", "Show only assets that match the input text"); }

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

	/** If bIncludeAssetPath is true, the text filter will match against full Asset path */
	void SetIncludeAssetPath(const bool InIncludeAssetPath);

	bool GetIncludeAssetPath() const;

private:

	/** Transient context data, used when calling PassesFilter. Kept around to minimize re-allocations between multiple calls to PassesFilter */
	TSharedRef<class FFrontendFilter_DialogueEntityFilterExpressionContext> TextFilterExpressionContext;

	/** Expression evaluator that can be used to perform complex text filter queries */
	FTextFilterExpressionEvaluator TextFilterExpressionEvaluator;
};

class FClassRestrictionFilter : public IFilter<FAssetFilterType>
{
public:
	FClassRestrictionFilter();

	// IFilter implementation
	virtual bool PassesFilter(FAssetFilterType InItem) const override;

	TSubclassOf<UArticyObject> AllowedClass;

	// IFilter implementation
	DECLARE_DERIVED_EVENT(FClassRestrictionFilter, IFilter<FAssetFilterType>::FChangedEvent, FChangedEvent);
	virtual FChangedEvent& OnChanged() override { return ChangedEvent; }

	FChangedEvent ChangedEvent;
};
#undef LOCTEXT_NAMESPACE