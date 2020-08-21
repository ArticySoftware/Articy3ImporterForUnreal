//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyRef.h"
#include "Framework/MultiBox/MultiBoxExtender.h"

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

class ARTICYEDITOR_API IArticyRefWidgetCustomizationFactory
{
public:
	virtual ~IArticyRefWidgetCustomizationFactory() {}
	virtual TSharedPtr<IArticyRefWidgetCustomization> CreateCustomization() = 0;
	virtual bool SupportsType(const FArticyRef& Ref) = 0;
};

DECLARE_DELEGATE_RetVal(TSharedRef<IArticyRefWidgetCustomizationFactory>, FOnCreateArticyRefWidgetCustomizationFactory);

/** Singleton-style class for managing articy editor customizations. */
class ARTICYEDITOR_API FArticyEditorCustomizationManager
{
public:
	FArticyEditorCustomizationManager();

	/** Registers an ArticyRefWidget customization factory and returns the index */
	IArticyRefWidgetCustomizationFactory* RegisterArticyRefWidgetCustomizationFactory(FOnCreateArticyRefWidgetCustomizationFactory GetCustomizationDelegate);
	/** Unregisters an ArticyRefWidget customization */
	void UnregisterArticyRefWidgetCustomizationFactory(const IArticyRefWidgetCustomizationFactory*);
	/** Creates the ArticyRefWidget customizations for an ArticyRef */
	void CreateArticyRefWidgetCustomizations(FArticyRef& ArticyRef, TArray<TSharedPtr<IArticyRefWidgetCustomization>>& OutCustomizations);

private:
	/** Since we only want one customization manager, delete other constructors */
	FArticyEditorCustomizationManager(const FArticyEditorCustomizationManager&) = delete;
	FArticyEditorCustomizationManager& operator=(const FArticyEditorCustomizationManager&) = delete;


	TArray<TSharedPtr<IArticyRefWidgetCustomizationFactory>> ArticyRefWidgetCustomizationFactories;
};

