//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#pragma once

#include "CoreMinimal.h"
#include <IPropertyTypeCustomization.h>
#include "ArticyRef.h"
#include "Slate/SArticyRefProperty.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "ArticyObject.h"

class FArticyRefClassFilter : public IClassViewerFilter
{
public:
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return InClass->IsChildOf(UArticyObject::StaticClass());
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return false;
	}
};

// reference: color struct customization
class FArticyRefCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	
	static FArticyRef* RetrieveArticyRef(IPropertyHandle* ArticyRefHandle);
	static FArticyId GetIdFromValueString(FString SourceString);

private:
	TSharedPtr<IPropertyHandle> ArticyRefPropertyHandle;
	TSharedPtr<SArticyRefProperty> ArticyRefProperty;
	UClass* ClassRestriction = nullptr;
	
	UClass* GetClassRestriction() const;
	FText GetChosenClassName() const;
	void OnClassPicked(UClass* InChosenClass);
	TSharedRef<SWidget> CreateClassPicker();


private:
	TSharedPtr<class SComboButton> ClassRestrictionButton;
};
