//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//
#pragma once

class UArticyObject;
class URichTextBlock;

#include "UObject/Interface.h"
#include "ArticyHyperlinkHandler.generated.h"

UINTERFACE(MinimalAPI)
class UArticyHyperlinkHandler : public UInterface { GENERATED_BODY() };

/**
  * Put this interface on a widget housing a rich text block with articy:draft hyperlinks (see UArticyRichTextDecorator).
  */
class IArticyHyperlinkHandler
{
    GENERATED_BODY()

public:
    /**
      * Called whenever a hyperlink is clicked
      */
    UFUNCTION(BlueprintImplementableEvent, Category = "articy")
    void OnHyperlinkNavigated(UArticyObject* Destination, URichTextBlock* Origin);
};