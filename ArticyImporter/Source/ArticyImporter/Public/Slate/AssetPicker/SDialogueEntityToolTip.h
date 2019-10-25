#pragma once
#include <SToolTip.h>
#include "ArticyObject.h"

class SDialogueEntityToolTip : public SToolTip
{
public:
	SLATE_BEGIN_ARGS(SDialogueEntityToolTip)
		: _ObjectToDisplay(nullptr)
	{ }

		SLATE_ARGUMENT(TWeakObjectPtr<UArticyObject>, ObjectToDisplay)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void OnOpening() override;
	virtual void OnClosed() override;

protected:

	TWeakObjectPtr<UArticyObject> ObjectToDisplay;

	TSharedRef<SWidget> CreateToolTipContent();
	/** Helper function for CreateToolTipWidget. Adds a key value pair to the info box of the tooltip */
	void AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, const FText& Value, bool bImportant) const;

private:
	/** The substring to be highlighted in the name and tooltip path */
	TAttribute<FText> HighlightText;
};