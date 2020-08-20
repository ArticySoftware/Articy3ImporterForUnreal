//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyRef.h"

/** Architecture Ref: SequenceCustomizationManager */

struct FArticyRefWidgetCustomizationInfo
{
	TSharedPtr<FExtender> ExtraButtonExtender;
};

class FArticyRefWidgetCustomizationBuilder
{
public:
	FArticyRefWidgetCustomizationBuilder(FArticyRef& InRef) : Ref(InRef)
	{}
	void AddCustomization(const FArticyRefWidgetCustomizationInfo& Customization) { Customizations.Add(Customization); }
	const TArray<FArticyRefWidgetCustomizationInfo>& GetCustomizations() const { return Customizations; }

	FArticyRef& GetArticyRef() { return Ref; }
private:
	FArticyRef& Ref;
	TArray<FArticyRefWidgetCustomizationInfo> Customizations;
};

class ARTICYEDITOR_API IArticyRefWidgetCustomization
{
public:
	virtual ~IArticyRefWidgetCustomization() {}
	/** Supply the Builder with a WidgetCustomizationInfo struct that extends the UI. Keep the widgets small (16x16 icons preferred) */
	virtual void RegisterArticyRefWidgetCustomization(FArticyRefWidgetCustomizationBuilder& Builder) = 0;
	virtual void UnregisterArticyRefWidgetCustomization() = 0;
};

DECLARE_DELEGATE_RetVal(TSharedRef<IArticyRefWidgetCustomization>, FOnGetArticyRefWidgetCustomizationFactory);

/** Singleton-style class for managing articy editor customizations. */
class ARTICYEDITOR_API FArticyEditorCustomizationManager
{
public:
	FArticyEditorCustomizationManager();

	/** Registers an ArticyRefWidget customization factory for a specific type of class */
	int32 RegisterInstancedArticyRefWidgetCustomization(TSubclassOf<UArticyObject> SupportedClass, FOnGetArticyRefWidgetCustomizationFactory GetCustomizationDelegate);
	/** Unregisters an ArticyRefWidget customization for a specific type of class */
	void UnregisterInstancedArticyRefWidgetCustomization(int32 Index);
	/** Gets the ArticyRefWidget customizations for an ArticyRef */
	void GetArticyRefWidgetCustomizations(FArticyRef& ArticyRef, TArray<TSharedPtr<IArticyRefWidgetCustomization>>& OutCustomizations);

private:
	/** Since we only want one customization manager, delete other constructors */
	FArticyEditorCustomizationManager(const FArticyEditorCustomizationManager&) = delete;
	FArticyEditorCustomizationManager& operator=(const FArticyEditorCustomizationManager&) = delete;

	struct FArticyRefWidgetCustomizationRegistryEntry
	{
		const UClass* SupportedClass;
		FOnGetArticyRefWidgetCustomizationFactory Factory;
	};

	TArray<FArticyRefWidgetCustomizationRegistryEntry> ArticyRefWidgetCustomizationEntries;
};

