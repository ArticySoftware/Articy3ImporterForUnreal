//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyRef.h"
#include "Framework/MultiBox/MultiBoxExtender.h"

/** Architecture Ref: SequenceCustomizationManager */

struct FArticyIdPropertyWidgetCustomizationInfo
{
	TSharedPtr<FExtender> ExtraButtonExtender;
};

class FArticyIdPropertyWidgetCustomizationBuilder
{
public:
	FArticyIdPropertyWidgetCustomizationBuilder(const UArticyObject* InObject) : ArticyObject(InObject)
	{}
	void AddCustomization(const FArticyIdPropertyWidgetCustomizationInfo& Customization) { Customizations.Add(Customization); }
	const TArray<FArticyIdPropertyWidgetCustomizationInfo>& GetCustomizations() const { return Customizations; }

	const UArticyObject* GetArticyObject() const { return ArticyObject; }
private:
	const UArticyObject* ArticyObject;
	TArray<FArticyIdPropertyWidgetCustomizationInfo> Customizations;
};

class ARTICYEDITOR_API IArticyIdPropertyWidgetCustomization
{
public:
	virtual ~IArticyIdPropertyWidgetCustomization() {}
	/** Supply the Builder with a WidgetCustomizationInfo struct that extends the UI. Keep the widgets small (16x16 icons preferred) */
	virtual void RegisterArticyIdPropertyWidgetCustomization(FArticyIdPropertyWidgetCustomizationBuilder& Builder) = 0;
	virtual void UnregisterArticyIdPropertyWidgetCustomization() = 0;
};

class ARTICYEDITOR_API IArticyIdPropertyWidgetCustomizationFactory
{
public:
	virtual ~IArticyIdPropertyWidgetCustomizationFactory() {}
	virtual TSharedPtr<IArticyIdPropertyWidgetCustomization> CreateCustomization() = 0;
	virtual bool SupportsType(const UArticyObject* InObject) = 0;
};

DECLARE_DELEGATE_RetVal(TSharedRef<IArticyIdPropertyWidgetCustomizationFactory>, FOnCreateArticyIdPropertyWidgetCustomizationFactory);

/** Singleton-style class for managing articy editor customizations. */
class ARTICYEDITOR_API FArticyEditorCustomizationManager
{
public:
	FArticyEditorCustomizationManager();

	/** Registers an ArticyIdPropertyWidget customization factory and returns the index */
	IArticyIdPropertyWidgetCustomizationFactory* RegisterArticyIdPropertyWidgetCustomizationFactory(FOnCreateArticyIdPropertyWidgetCustomizationFactory GetCustomizationDelegate);
	/** Unregisters an ArticyIdPropertyyWidget customization */
	void UnregisterArticyIdPropertyWidgetCustomizationFactory(const IArticyIdPropertyWidgetCustomizationFactory*);
	/** Creates the ArticyIdPropertyWidget customizations for an ArticyRef */
	void CreateArticyIdPropertyWidgetCustomizations(const UArticyObject* ArticyObject, TArray<TSharedPtr<IArticyIdPropertyWidgetCustomization>>& OutCustomizations);

private:
	/** Since we only want one customization manager, delete other constructors */
	FArticyEditorCustomizationManager(const FArticyEditorCustomizationManager&) = delete;
	FArticyEditorCustomizationManager& operator=(const FArticyEditorCustomizationManager&) = delete;


	TArray<TSharedPtr<IArticyIdPropertyWidgetCustomizationFactory>> ArticyIdPropertyWidgetCustomizationFactories;
};

