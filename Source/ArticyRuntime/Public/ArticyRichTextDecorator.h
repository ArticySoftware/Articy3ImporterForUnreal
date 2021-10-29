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

UCLASS()
class ARTICYRUNTIME_API UArticyRichTextDecorator : public URichTextBlockDecorator
{
    GENERATED_BODY()

public:
    UArticyRichTextDecorator(const FObjectInitializer& ObjectInitializer);

    virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
};