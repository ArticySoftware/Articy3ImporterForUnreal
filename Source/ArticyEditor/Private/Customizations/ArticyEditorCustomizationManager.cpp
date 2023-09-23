//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Customizations/ArticyEditorCustomizationManager.h"
#include "ArticyObject.h"

FArticyEditorCustomizationManager::FArticyEditorCustomizationManager()
{
}

IArticyIdPropertyWidgetCustomizationFactory* FArticyEditorCustomizationManager::RegisterArticyIdPropertyWidgetCustomizationFactory(FOnCreateArticyIdPropertyWidgetCustomizationFactory GetCustomizationDelegate)
{
	ArticyIdPropertyWidgetCustomizationFactories.Add(GetCustomizationDelegate.Execute());
	return ArticyIdPropertyWidgetCustomizationFactories.Last().Get();
}

void FArticyEditorCustomizationManager::UnregisterArticyIdPropertyWidgetCustomizationFactory(const IArticyIdPropertyWidgetCustomizationFactory* Factory)
{
#if __cplusplus >= 202002L
	const int32 RemovedElements = ArticyIdPropertyWidgetCustomizationFactories.RemoveAll([=, this](const TSharedPtr<IArticyIdPropertyWidgetCustomizationFactory> FactoryPtr)
#else
	const int32 RemovedElements = ArticyIdPropertyWidgetCustomizationFactories.RemoveAll([=](const TSharedPtr<IArticyIdPropertyWidgetCustomizationFactory> FactoryPtr)
#endif
	{
		return Factory == FactoryPtr.Get();
	});

	ensureMsgf(RemovedElements != 0, TEXT("Failed removing factory. It was either not registered or removed already."));
}

void FArticyEditorCustomizationManager::CreateArticyIdPropertyWidgetCustomizations(const UArticyObject* ArticyObject, TArray<TSharedPtr<IArticyIdPropertyWidgetCustomization>>& OutCustomizations)
{
	if(ArticyObject == nullptr)
	{
		return;
	}
	
	for (const TSharedPtr<IArticyIdPropertyWidgetCustomizationFactory>& Entry : ArticyIdPropertyWidgetCustomizationFactories)
	{
		if(Entry->SupportsType(ArticyObject))
		{
			OutCustomizations.Add(Entry->CreateCustomization());
		}
	}
}

void FArticyEditorCustomizationManager::Shutdown()
{
	ArticyIdPropertyWidgetCustomizationFactories.Empty();
}
