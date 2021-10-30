//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyRichTextDecorator.h"
#include "UObject/SoftObjectPtr.h"
#include "Rendering/DrawElements.h"
#include "Framework/Text/SlateTextRun.h"
#include "Framework/Text/SlateTextLayout.h"
#include "Slate/SlateGameResources.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"
#include "Math/UnrealMathUtility.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SBox.h"
#include "Misc/DefaultValueHelper.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Package.h"
#include "Components/RichTextBlock.h"

class FArticyRichTextDecorator : public FRichTextDecorator
{
public:
	FArticyRichTextDecorator(URichTextBlock* InOwner)
		: FRichTextDecorator(InOwner)
	{
		for (TFieldIterator<UProperty> It(URichTextBlock::StaticClass()); It; ++It)
		{
			if (It->GetNameCPP() == TEXT("TextStyleSet"))
			{
				DataTableProp = *It;
			}
		}
	}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if (RunParseResult.MetaData.Contains(TEXT("color")))
		{
			return true;
		}

		return false;
	}

protected:
    virtual void CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const override
    {
		// Add text to string
		InOutString += RunInfo.Content.ToString();

		// Check if this style is defined in our table
		UDataTable* StyleTable = GetStyleTable();
		if (StyleTable)
		{
			// If it is, use its text styling by copying the row in
			const FRichTextStyleRow* row = StyleTable->FindRow<FRichTextStyleRow>(*RunInfo.Name, "ArticyStyleTableCheck");
			if (row)
			{
				InOutTextStyle = row->TextStyle;
			}
		}

		// If we have a color attribute
		if (RunInfo.MetaData.Contains(TEXT("color"))) {
			// Change the style color
			const FString color = RunInfo.MetaData[TEXT("color")];
			InOutTextStyle.ColorAndOpacity = FSlateColor(FColor::FromHex(color));
		}
    }

private:
	// Cached pointer to data table property in URichTextBlock. Needed because the property is protected :(
	FProperty* DataTableProp = nullptr;

	// Gets the style table from the owning rich text block
	UDataTable* GetStyleTable() const
	{
		if (DataTableProp == nullptr) {
			return nullptr;
		}
		return *DataTableProp->ContainerPtrToValuePtr<UDataTable*>(Owner, 0);
	}
};

UArticyRichTextDecorator::UArticyRichTextDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UArticyRichTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	return MakeShareable(new FArticyRichTextDecorator(InOwner));
}