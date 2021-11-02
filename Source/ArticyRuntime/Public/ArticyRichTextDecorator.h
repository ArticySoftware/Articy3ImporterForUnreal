//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Fonts/SlateFontInfo.h"
#include "Styling/SlateTypes.h"
#include "Framework/Text/TextLayout.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/ITextDecorator.h"
#include "Components/RichTextBlockDecorator.h"
#include "Engine/DataTable.h"
#include "ArticyRichTextDecorator.generated.h"

class UArticyObject;
class IArticyHyperlinkHandler;

UCLASS()
class ARTICYRUNTIME_API UArticyRichTextDecorator : public URichTextBlockDecorator
{
    GENERATED_BODY()

public:
    UArticyRichTextDecorator(const FObjectInitializer& ObjectInitializer);

    // Creates a new text decorator instance for a rich text block
    virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

    // Callback when a link is navigated
	void OnArticyLinkNavigated(URichTextBlock* Parent, const FString Link);

    // Used to find the hyperlink handler parent of a rich text block
    IArticyHyperlinkHandler* GetHyperlinkHandler(URichTextBlock* RichTextBlock);

    // Used to match an articy object to a hyperlink url
    UArticyObject* GetLinkDestination(URichTextBlock* Owner, const FString& Link);
};