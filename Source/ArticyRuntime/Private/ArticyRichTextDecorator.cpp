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

class FArticyRichTextDecorator : public FRichTextDecorator
{
public:
	FArticyRichTextDecorator(URichTextBlock* InOwner)
		: FRichTextDecorator(InOwner)
	{
	}

	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override
	{
		if ((RunParseResult.Name == TEXT("color") 
			|| RunParseResult.Name == TEXT("align")
			)
			&& RunParseResult.MetaData.Contains(TEXT("value")))
		{
			return true;
		}
		if (RunParseResult.Name == TEXT("b")) {
			return true;
		}

		return false;
	}

protected:
    virtual void CreateDecoratorText(const FTextRunInfo& RunInfo, FTextBlockStyle& InOutTextStyle, FString& InOutString) const override
    {
		// Add text to string
		InOutString += RunInfo.Content.ToString();

		// Early return
		if (!RunInfo.MetaData.Contains(TEXT("value"))) {
			return;
		}
		
		// Get value
		const FString value = RunInfo.MetaData[TEXT("value")];

		// Style alignment
		if (RunInfo.Name == TEXT("align"))
		{
			// nothing to do. Unreal's Rich Text Box doesn't support it :'(
		}
		// Color styling
		else if (RunInfo.Name == TEXT("color"))
		{
			InOutTextStyle.ColorAndOpacity = FSlateColor(FColor::FromHex(value));
		}
    }
};

UArticyRichTextDecorator::UArticyRichTextDecorator(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSharedPtr<ITextDecorator> UArticyRichTextDecorator::CreateDecorator(URichTextBlock* InOwner)
{
	/*FRichTextStyleRow* x = InOwner->TextStyleSet->FindRow<FRichTextStyleRow>(NAME_Actor, "Hello");
	x->TextStyle.Font.*/
	return MakeShareable(new FArticyRichTextDecorator(InOwner));
}