//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//


#include "Slate/ArticyFilterHelpers.h"
#include "ArticyObject.h"
#include "Interfaces/ArticyObjectWithDisplayName.h"
#include "Interfaces/ArticyObjectWithText.h"
#include "Interfaces/ArticyObjectWithSpeaker.h"
#include "ArticyEditorModule.h"

#define LOCTEXT_NAMESPACE "ArticyObjectSearchBoxHelpers"

/** Expression context to test the given asset data against the current text filter */
class FFrontendFilter_ArticyObjectFilterExpressionContext : public ITextFilterExpressionContext
{
public:
	typedef TRemoveReference<FArticyObjectFilterType>::Type* FArticyObjectFilterTypePtr;

	FFrontendFilter_ArticyObjectFilterExpressionContext()
		: AssetPtr(nullptr)
		, bIncludeClassName(true)
		, NameKeyName("Name")
		, PathKeyName("Path")
		, ClassKeyName("Class")
		, TypeKeyName("Type")
		, TagKeyName("Tag")
	{
	}

	void SetAsset(FArticyObjectFilterTypePtr InAsset)
	{
		AssetPtr = InAsset;
	}

	void ClearAsset()
	{
		AssetPtr = nullptr;
	}

	void SetIncludeClassName(const bool InIncludeClassName)
	{
		bIncludeClassName = InIncludeClassName;
	}

	bool GetIncludeClassName() const
	{
		return bIncludeClassName;
	}

	virtual bool TestBasicStringExpression(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const override
	{
		UArticyObject* ArticyObject = Cast<UArticyObject>(AssetPtr->GetAsset());

		if(CheckDisplayName(InValue, InTextComparisonMode))
		{
			return true;
		}

		if (CheckTechnicalName(InValue, InTextComparisonMode))
		{
			return true;
		}

		if (CheckTextOfArticyObject(InValue, InTextComparisonMode))
		{
			return true;
		}

		if(CheckSpeakerDisplayName(InValue, InTextComparisonMode))
		{
			return true;
		}

		const FTextFilterString AssetName(AssetPtr->AssetName);
		if (AssetName.CompareText(InValue, InTextComparisonMode))
		{
			return true;
		}

		if (bIncludeClassName)
		{
			const FTextFilterString AssetClassFilter(AssetPtr->AssetClass);
			if (AssetClassFilter.CompareText(InValue, InTextComparisonMode))
			{
				return true;
			}
		}

		return false;
	}

	virtual bool TestComplexExpression(const FName& InKey, const FTextFilterString& InValue, const ETextFilterComparisonOperation InComparisonOperation, const ETextFilterTextComparisonMode InTextComparisonMode) const override
	{
		// Special case for the asset name, as this isn't contained within the asset registry meta-data
		if (InKey == NameKeyName)
		{
			// Names can only work with Equal or NotEqual type tests
			if (InComparisonOperation != ETextFilterComparisonOperation::Equal && InComparisonOperation != ETextFilterComparisonOperation::NotEqual)
			{
				return false;
			}

			const bool bIsMatch = TextFilterUtils::TestBasicStringExpression(AssetPtr->AssetName, InValue, InTextComparisonMode);
			return (InComparisonOperation == ETextFilterComparisonOperation::Equal) ? bIsMatch : !bIsMatch;
		}

		// Special case for the asset path, as this isn't contained within the asset registry meta-data
		if (InKey == PathKeyName)
		{
			// Paths can only work with Equal or NotEqual type tests
			if (InComparisonOperation != ETextFilterComparisonOperation::Equal && InComparisonOperation != ETextFilterComparisonOperation::NotEqual)
			{
				return false;
			}

			// If the comparison mode is partial, then we only need to test the ObjectPath as that contains the other two as sub-strings
			bool bIsMatch = false;
			if (InTextComparisonMode == ETextFilterTextComparisonMode::Partial)
			{
				bIsMatch = TextFilterUtils::TestBasicStringExpression(AssetPtr->ObjectPath, InValue, InTextComparisonMode);
			}
			else
			{
				bIsMatch = TextFilterUtils::TestBasicStringExpression(AssetPtr->ObjectPath, InValue, InTextComparisonMode)
					|| TextFilterUtils::TestBasicStringExpression(AssetPtr->PackageName, InValue, InTextComparisonMode)
					|| TextFilterUtils::TestBasicStringExpression(AssetPtr->PackagePath, InValue, InTextComparisonMode);
			}
			return (InComparisonOperation == ETextFilterComparisonOperation::Equal) ? bIsMatch : !bIsMatch;
		}

		// Special case for the asset type, as this isn't contained within the asset registry meta-data
		if (InKey == ClassKeyName || InKey == TypeKeyName)
		{
			// Class names can only work with Equal or NotEqual type tests
			if (InComparisonOperation != ETextFilterComparisonOperation::Equal && InComparisonOperation != ETextFilterComparisonOperation::NotEqual)
			{
				return false;
			}

			const bool bIsMatch = TextFilterUtils::TestBasicStringExpression(AssetPtr->AssetClass, InValue, InTextComparisonMode);
			return (InComparisonOperation == ETextFilterComparisonOperation::Equal) ? bIsMatch : !bIsMatch;
		}

		// Generic handling for anything in the asset meta-data
		{
			auto GetMetaDataValue = [this, &InKey](FString & OutMetaDataValue) -> bool
			{
				// Check for a literal key
				if (AssetPtr->GetTagValue(InKey, OutMetaDataValue))
				{
					return true;
				}

				return false;
			};

			FString MetaDataValue;
			if (GetMetaDataValue(MetaDataValue))
			{
				return TextFilterUtils::TestComplexExpression(MetaDataValue, InValue, InComparisonOperation, InTextComparisonMode);
			}
		}

		return false;
	}

private:

	/** Pointer to the asset we're currently filtering */
	FArticyObjectFilterTypePtr AssetPtr;

	/** Are we supposed to include the class name in our basic string tests? */
	bool bIncludeClassName;

	/** Keys used by TestComplexExpression */
	const FName NameKeyName;
	const FName PathKeyName;
	const FName ClassKeyName;
	const FName TypeKeyName;
	const FName TagKeyName;

	bool CheckDisplayName(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const
	{
		IArticyObjectWithDisplayName* ArticyObjectWithDisplayName = Cast<IArticyObjectWithDisplayName>(AssetPtr->GetAsset());

		if(ArticyObjectWithDisplayName)
		{
			const FTextFilterString TextToCompare(ArticyObjectWithDisplayName->GetDisplayName().ToString());

			if (TextToCompare.IsEmpty())
			{
				return false;
			}

			return TextToCompare.CompareText(InValue, InTextComparisonMode);
		}

		return false;
	}

	bool CheckTextOfArticyObject(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const
	{
		IArticyObjectWithText* ArticyObjectWithText = Cast<IArticyObjectWithText>(AssetPtr->GetAsset());

		if(ArticyObjectWithText)
		{
			const FText Text = ArticyObjectWithText->GetText();
			const FTextFilterString FilterString(Text.ToString());
		
			const bool bTextIsEmptyOrWhitespace = Text.IsEmptyOrWhitespace();
			
			if(!bTextIsEmptyOrWhitespace && FilterString.CompareText(InValue, InTextComparisonMode))
			{
				return true;
			}
		}

		return false;
	}

	bool CheckTechnicalName(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const
	{
		UArticyObject* ArticyObject = Cast<UArticyObject>(AssetPtr->GetAsset());

		if (ArticyObject)
		{
			const FTextFilterString TextToCompare(ArticyObject->GetTechnicalName());
			
			if (TextToCompare.IsEmpty())
			{
				return false;
			}

			return TextToCompare.CompareText(InValue, InTextComparisonMode);
		}

		return false;
	}

	bool CheckSpeakerDisplayName(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const
	{
		IArticyObjectWithSpeaker* ArticyObjectWithSpeaker = Cast<IArticyObjectWithSpeaker>(AssetPtr->GetAsset());

		if(ArticyObjectWithSpeaker)
		{
			IArticyObjectWithDisplayName* SpeakerDisplayName = Cast<IArticyObjectWithDisplayName>(UArticyObject::FindAsset(ArticyObjectWithSpeaker->GetSpeakerId()));

			if(!SpeakerDisplayName)
			{
				UE_LOG(LogArticyEditor, Error, TEXT("Articy filter: Speaker object does not exist"));
				return false;
			}

			FText& SpeakerName = SpeakerDisplayName->GetDisplayName();
			const FTextFilterString TextToCompare(SpeakerName.ToString());

			if (TextToCompare.IsEmpty())
			{
				return false;
			}

			return TextToCompare.CompareText(InValue, InTextComparisonMode);
		}
		

		return false;
	}
};


FFrontendFilter_ArticyObject::FFrontendFilter_ArticyObject() :
	 TextFilterExpressionContext(MakeShareable(new FFrontendFilter_ArticyObjectFilterExpressionContext()))
	, TextFilterExpressionEvaluator(ETextFilterExpressionEvaluatorMode::Complex)
{
}

FFrontendFilter_ArticyObject::~FFrontendFilter_ArticyObject()
{

}

bool FFrontendFilter_ArticyObject::PassesFilter(FArticyObjectFilterType InItem) const
{
	TextFilterExpressionContext->SetAsset(&InItem);
	const bool bMatched = TextFilterExpressionEvaluator.TestTextFilter(*TextFilterExpressionContext);
	TextFilterExpressionContext->ClearAsset();
	return bMatched;
}

FText FFrontendFilter_ArticyObject::GetRawFilterText() const
{
	return TextFilterExpressionEvaluator.GetFilterText();
}

void FFrontendFilter_ArticyObject::SetRawFilterText(const FText& InFilterText)
{
	if (TextFilterExpressionEvaluator.SetFilterText(InFilterText))
	{
		// Will trigger a re-filter with the new text
		BroadcastChangedEvent();
	}
}

FText FFrontendFilter_ArticyObject::GetFilterErrorText() const
{
	return TextFilterExpressionEvaluator.GetFilterErrorText();
}

void FFrontendFilter_ArticyObject::SetIncludeClassName(const bool InIncludeClassName)
{
	if (TextFilterExpressionContext->GetIncludeClassName() != InIncludeClassName)
	{
		TextFilterExpressionContext->SetIncludeClassName(InIncludeClassName);

		// Will trigger a re-filter with the new setting
		BroadcastChangedEvent();
	}
}

FArticyClassRestrictionFilter::FArticyClassRestrictionFilter(TSubclassOf<UArticyObject> InAllowedClass, bool bInExactClass) : AllowedClass(InAllowedClass), bExactClass(bInExactClass)
{
}

bool FArticyClassRestrictionFilter::PassesFilter(FArticyObjectFilterType InItem) const
{
	if(bExactClass)
	{
		return InItem.GetClass() == AllowedClass;
	}
	
	return InItem.GetAsset()->IsA(AllowedClass.Get());
}


class FFrontendFilter_ArticyGVFilterExpressionContext : public ITextFilterExpressionContext
{
public:
	typedef TRemoveReference<FArticyVariableFilterType>::Type FArticyVariableFilterTypePtr;

	FFrontendFilter_ArticyGVFilterExpressionContext()
		: VariablePtr(nullptr)
	{}

	void SetVariable(FArticyVariableFilterTypePtr InVariable)
	{
		VariablePtr = InVariable;
	}
	void ClearVariable()
	{
		VariablePtr = nullptr;
	}

	virtual bool TestBasicStringExpression(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const override;
	virtual bool TestComplexExpression(const FName& InKey, const FTextFilterString& InValue,
		const ETextFilterComparisonOperation InComparisonOperation, const ETextFilterTextComparisonMode InTextComparisonMode) const override;
private:

	FArticyVariableFilterTypePtr VariablePtr;
};

bool FFrontendFilter_ArticyGVFilterExpressionContext::TestBasicStringExpression(const FTextFilterString& InValue, const ETextFilterTextComparisonMode InTextComparisonMode) const
{
	const UArticyVariable* Variable = VariablePtr;
	
	const FTextFilterString VarNameToCompare(Variable->GetName());
	if(VarNameToCompare.CompareText(InValue, InTextComparisonMode))
	{
		return true;
	}

	const FTextFilterString VarSetNameToCompare = Variable->GetOuter()->GetName();
	if(VarSetNameToCompare.CompareText(InValue, InTextComparisonMode))
	{
		return true;
	}

	return false;
}

bool FFrontendFilter_ArticyGVFilterExpressionContext::TestComplexExpression(const FName& InKey, const FTextFilterString& InValue,
	const ETextFilterComparisonOperation InComparisonOperation, const ETextFilterTextComparisonMode InTextComparisonMode) const
{
	// dont use this filter
	return false;
}

FFrontendFilter_ArticyVariable::FFrontendFilter_ArticyVariable() :
	TextFilterExpressionContext(MakeShareable(new FFrontendFilter_ArticyGVFilterExpressionContext())),
	TextFilterExpressionEvaluator(ETextFilterExpressionEvaluatorMode::BasicString)
{
}

FFrontendFilter_ArticyVariable::~FFrontendFilter_ArticyVariable()
{
}

bool FFrontendFilter_ArticyVariable::PassesFilter(FArticyVariableFilterType InItem) const
{
	TextFilterExpressionContext->SetVariable(InItem);
	const bool bMatched = TextFilterExpressionEvaluator.TestTextFilter(*TextFilterExpressionContext);
	TextFilterExpressionContext->ClearVariable();
	return bMatched;
}

FText FFrontendFilter_ArticyVariable::GetRawFilterText() const
{
	return TextFilterExpressionEvaluator.GetFilterText();
}

void FFrontendFilter_ArticyVariable::SetRawFilterText(const FText& InFilterText)
{
	if (TextFilterExpressionEvaluator.SetFilterText(InFilterText))
	{
		// Will trigger a re-filter with the new text
		ChangedEvent.Broadcast();
	}
}

FText FFrontendFilter_ArticyVariable::GetFilterErrorText() const
{
	return TextFilterExpressionEvaluator.GetFilterErrorText();
}

#undef LOCTEXT_NAMESPACE

