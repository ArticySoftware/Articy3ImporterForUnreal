// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectSearchBoxHelpers.h"
#include "ArticyObject.h"
#include "ArticyObjectWithDisplayName.h"
#include "ArticyObjectWithText.h"

#define LOCTEXT_NAMESPACE "DialogueEntityTileView"

/** Expression context to test the given asset data against the current text filter */
class FFrontendFilter_DialogueEntityFilterExpressionContext : public ITextFilterExpressionContext
{
public:
	typedef TRemoveReference<FAssetFilterType>::Type* FAssetFilterTypePtr;

	FFrontendFilter_DialogueEntityFilterExpressionContext()
		: AssetPtr(nullptr)
		, bIncludeClassName(true)
		, bIncludeAssetPath(false)
		, NameKeyName("Name")
		, PathKeyName("Path")
		, ClassKeyName("Class")
		, TypeKeyName("Type")
		, TagKeyName("Tag")
	{
	}

	void SetAsset(FAssetFilterTypePtr InAsset)
	{
		AssetPtr = InAsset;

		if (bIncludeAssetPath)
		{
			// Get the full asset path, and also split it so we can compare each part in the filter
			AssetPtr->PackageName.AppendString(AssetFullPath);
			AssetFullPath.ParseIntoArray(AssetSplitPath, TEXT("/"));
			AssetFullPath.ToUpperInline();

			if (bIncludeClassName)
			{
				// Get the full export text path as people sometimes search by copying this (requires class and asset path search to be enabled in order to match)
				AssetPtr->GetExportTextName(AssetExportTextName);
				AssetExportTextName.ToUpperInline();
			}
		}
	}

	void ClearAsset()
	{
		AssetPtr = nullptr;
		AssetFullPath.Reset();
		AssetExportTextName.Reset();
		AssetSplitPath.Reset();
	}

	void SetIncludeClassName(const bool InIncludeClassName)
	{
		bIncludeClassName = InIncludeClassName;
	}

	bool GetIncludeClassName() const
	{
		return bIncludeClassName;
	}

	void SetIncludeAssetPath(const bool InIncludeAssetPath)
	{
		bIncludeAssetPath = InIncludeAssetPath;
	}

	bool GetIncludeAssetPath() const
	{
		return bIncludeAssetPath;
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

		FTextFilterString AssetName(AssetPtr->AssetName);
		if (AssetName.CompareText(InValue, InTextComparisonMode))
		{
			return true;
		}

		if (bIncludeAssetPath)
		{
			FTextFilterString AssetFullPathFilter(AssetFullPath);
			if (AssetFullPathFilter.CompareText(InValue, InTextComparisonMode))
			{
				return true;
			}

			for (const FString& AssetPathPart : AssetSplitPath)
			{
				FTextFilterString AssetPathPartFilter(AssetPathPart);
				if (AssetPathPartFilter.CompareText(InValue, InTextComparisonMode))
				{
					return true;
				}
			}
		}

		if (bIncludeClassName)
		{
			FTextFilterString AssetClassFilter(AssetPtr->AssetClass);
			if (AssetClassFilter.CompareText(InValue, InTextComparisonMode))
			{
				return true;
			}
		}

		if (bIncludeClassName && bIncludeAssetPath)
		{
			// Only test this if we're searching the class name and asset path too, as the exported text contains the type and path in the string
			FTextFilterString AssetExportTextNameFilter(AssetExportTextName);
			if (AssetExportTextNameFilter.CompareText(InValue, InTextComparisonMode))
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
	FAssetFilterTypePtr AssetPtr;

	/** Full path of the current asset */
	FString AssetFullPath;

	/** The export text name of the current asset */
	FString AssetExportTextName;

	/** Split path of the current asset */
	TArray<FString> AssetSplitPath;

	/** Are we supposed to include the class name in our basic string tests? */
	bool bIncludeClassName;

	/** Search inside the entire asset path? */
	bool bIncludeAssetPath;

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
			FTextFilterString TextToCompare(ArticyObjectWithDisplayName->GetDisplayName().ToString());

			if (TextToCompare.IsEmpty())
			{
				return false;
			}

			return TextToCompare.CompareText(InValue, InTextComparisonMode);

			// CompareFString and CompareText somehow seem to return incorrect values
			//return InValue.CompareFString(displayName, InTextComparisonMode);
			//return InValue.CompareText(TextToCompare, InTextComparisonMode);
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
			FTextFilterString TextToCompare(ArticyObject->GetTechnicalName());
			
			if (TextToCompare.IsEmpty())
			{
				return false;
			}

			return TextToCompare.CompareText(InValue, InTextComparisonMode);
		}

		return false;
	}
};


FFrontendFilter_DialogueEntity::FFrontendFilter_DialogueEntity()
	: FFrontendFilter(nullptr)
	, TextFilterExpressionContext(MakeShareable(new FFrontendFilter_DialogueEntityFilterExpressionContext()))
	, TextFilterExpressionEvaluator(ETextFilterExpressionEvaluatorMode::Complex)
{
}

FFrontendFilter_DialogueEntity::~FFrontendFilter_DialogueEntity()
{

}

bool FFrontendFilter_DialogueEntity::PassesFilter(FAssetFilterType InItem) const
{
	TextFilterExpressionContext->SetAsset(&InItem);
	const bool bMatched = TextFilterExpressionEvaluator.TestTextFilter(*TextFilterExpressionContext);
	TextFilterExpressionContext->ClearAsset();
	return bMatched;
}

FText FFrontendFilter_DialogueEntity::GetRawFilterText() const
{
	return TextFilterExpressionEvaluator.GetFilterText();
}

void FFrontendFilter_DialogueEntity::SetRawFilterText(const FText& InFilterText)
{
	if (TextFilterExpressionEvaluator.SetFilterText(InFilterText))
	{
		// Will trigger a re-filter with the new text
		BroadcastChangedEvent();
	}
}

FText FFrontendFilter_DialogueEntity::GetFilterErrorText() const
{
	return TextFilterExpressionEvaluator.GetFilterErrorText();
}

void FFrontendFilter_DialogueEntity::SetIncludeClassName(const bool InIncludeClassName)
{
	if (TextFilterExpressionContext->GetIncludeClassName() != InIncludeClassName)
	{
		TextFilterExpressionContext->SetIncludeClassName(InIncludeClassName);

		// Will trigger a re-filter with the new setting
		BroadcastChangedEvent();
	}
}

void FFrontendFilter_DialogueEntity::SetIncludeAssetPath(const bool InIncludeAssetPath)
{
	if (TextFilterExpressionContext->GetIncludeAssetPath() != InIncludeAssetPath)
	{
		TextFilterExpressionContext->SetIncludeAssetPath(InIncludeAssetPath);

		// Will trigger a re-filter with the new setting
		BroadcastChangedEvent();
	}
}

bool FFrontendFilter_DialogueEntity::GetIncludeAssetPath() const
{
	return TextFilterExpressionContext->GetIncludeAssetPath();
}

FClassRestrictionFilter::FClassRestrictionFilter()
{
}

bool FClassRestrictionFilter::PassesFilter(FAssetFilterType InItem) const
{
	return InItem.GetAsset()->IsA(AllowedClass.Get());
}

#undef LOCTEXT_NAMESPACE

