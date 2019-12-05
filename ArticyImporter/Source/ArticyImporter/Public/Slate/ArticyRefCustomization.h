#pragma once

#include "CoreMinimal.h"
#include <IPropertyTypeCustomization.h>
#include <DetailLayoutBuilder.h>
#include <IDetailCustomization.h>
#include "ArticyRef.h"
#include "SDialogueEntityProperty.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "ArticyObject.h"

class FArticyRefClassFilter : public IClassViewerFilter
{
public:
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return InClass->IsChildOf(UArticyObject::StaticClass());
		//	InFilterFuncs->IfInChildOfClassesSet()
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		// @TODO: One day would be nice to see functions on unloaded classes...
		return false;
	}
};

class FArticyRefCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	TSharedPtr<IPropertyHandle> ArticyRefPropertyHandle;
	TSharedPtr<SArticyRefSelection> ArticyRefSelection;
	UClass* ClassRestriction = nullptr;

	FArticyRef* RetrieveArticyRef() const;
	UClass* GetClassRestriction() const;
	FText GetChosenClassName() const;
	void OnClassPicked(UClass* InChosenClass);
	TSharedRef<SWidget> CreateClassPicker() const;

private:
	/** A pointer to a class viewer **/
	TSharedPtr<class SClassViewer> ClassViewer;
	TSharedPtr<class SComboButton> ClassRestrictionButton;
};

