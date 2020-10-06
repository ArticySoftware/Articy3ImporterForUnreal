//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Customizations/ArticyEditorCustomizationManager.h"
#include "Input/Reply.h"

class FArticyButtonCustomization : public IArticyIdPropertyWidgetCustomization
{
public:
	virtual void RegisterArticyIdPropertyWidgetCustomization(FArticyIdPropertyWidgetCustomizationBuilder& Builder) override;
	virtual void UnregisterArticyIdPropertyWidgetCustomization() override;

	void CreateArticyButton(FToolBarBuilder& Builder);
	FReply OnArticyButtonClicked();

private:
	TWeakObjectPtr<const UArticyObject> ArticyObject = nullptr;
};

class FArticyButtonCustomizationFactory : public IArticyIdPropertyWidgetCustomizationFactory
{
public:
	virtual TSharedPtr<IArticyIdPropertyWidgetCustomization> CreateCustomization() override;
	virtual bool SupportsType(const UArticyObject* ArticyObject) override;
};
