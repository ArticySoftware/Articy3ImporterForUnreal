//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "Customizations/ArticyEditorCustomizationManager.h"
#include "ArticyObject.h"

FArticyEditorCustomizationManager::FArticyEditorCustomizationManager()
{
}

int32 FArticyEditorCustomizationManager::RegisterInstancedArticyRefWidgetCustomization(TSubclassOf<UArticyObject> SupportedClass,
                                                                                       FOnGetArticyRefWidgetCustomizationInstance GetCustomizationDelegate)
{
	if(!ensure(IsValid(SupportedClass)))
	{
		return -1;
	}
	
	FArticyRefWidgetCustomizationRegistryEntry Entry;
	Entry.SupportedClass = SupportedClass;
	Entry.Factory = GetCustomizationDelegate;

	return ArticyRefWidgetCustomizationEntries.Add(Entry);
}

void FArticyEditorCustomizationManager::UnregisterInstancedArticyRefWidgetCustomization(int32 Index)
{
	if(!ensure(ArticyRefWidgetCustomizationEntries.IsValidIndex(Index)))
	{
		return;
	}

	ArticyRefWidgetCustomizationEntries.RemoveAt(Index);
}

void FArticyEditorCustomizationManager::GetArticyRefWidgetCustomizations(FArticyRef& ArticyRef, TArray<TSharedPtr<IArticyRefWidgetCustomization>>& OutCustomizations)
{
	UArticyObject* Object = UArticyObject::FindAsset(ArticyRef.GetId());

	if(Object == nullptr)
	{
		return;
	}

	TSubclassOf<UArticyObject> SupportedClass = Object->UObject::GetClass();
	
	TArray<FArticyRefWidgetCustomizationRegistryEntry> FilteredCustomizations = ArticyRefWidgetCustomizationEntries.FilterByPredicate([=](const FArticyRefWidgetCustomizationRegistryEntry& Entry)
	{
		return SupportedClass.Get()->IsChildOf(Entry.SupportedClass);
	});

	for (const FArticyRefWidgetCustomizationRegistryEntry& Entry : FilteredCustomizations)
	{
		TSharedRef<IArticyRefWidgetCustomization> Instance(Entry.Factory.Execute());
		OutCustomizations.Add(Instance);
	}
}
