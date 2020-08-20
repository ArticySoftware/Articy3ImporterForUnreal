//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Customizations/ArticyEditorCustomizationManager.h"
#include "ArticyRef.h"

class FArticyButtonCustomization : public IArticyRefWidgetCustomization
{
public:
	virtual void RegisterArticyRefWidgetCustomization(FArticyRefWidgetCustomizationBuilder& Builder) override;
	virtual void UnregisterArticyRefWidgetCustomization() override;

	void CreateArticyButton(FToolBarBuilder& Builder);
	FReply OnArticyButtonClicked();

private:
	FArticyRef* Ref = nullptr;
};

class FArticyButtonCustomizationFactory : public IArticyRefWidgetCustomizationFactory
{
public:
	virtual TSharedPtr<IArticyRefWidgetCustomization> CreateCustomization() override;
	virtual bool SupportsType(const FArticyRef& Ref) override;
};
