//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include <IPropertyTypeCustomization.h>
#include "ArticyRef.h"
#include "Slate/SArticyIdProperty.h"
#include "ClassViewerFilter.h"
#include "ClassViewerModule.h"
#include "ArticyObject.h"

class FArticyRefClassFilter : public IClassViewerFilter
{
public:
	FArticyRefClassFilter(UClass* GivenClass = UArticyObject::StaticClass(), bool bInRequiresExactClass = false);
	
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		if(bRequiresExactClass)
		{
			return InClass == GivenClass;
		}
		
		return InClass->IsChildOf(GivenClass);
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		return false;
	}

	UClass* GivenClass = nullptr;
	bool bRequiresExactClass = false;
};

// reference: color struct customization
class FArticyIdCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	static FArticyId* RetrieveArticyId(IPropertyHandle* ArticyIdHandle);
	static FArticyId GetIdFromValueString(FString SourceString);

private:
	TSharedPtr<IPropertyHandle> ArticyIdPropertyHandle;
	TSharedPtr<SArticyIdProperty> ArticyIdPropertyWidget;
	UClass* ClassRestriction = nullptr;
	/** Used when there are multiple objects to edit */
	bool bIsEditable = false;
private:
	UClass* GetClassRestriction() const;
	UClass* GetClassRestrictionMetaData() const;
	bool IsExactClass() const;
	bool IsEditable() const { return bIsEditable; }
	bool HasClassRestrictionMetaData() const;
	bool HasExactClassMetaData() const;

	FArticyId GetArticyId() const;
	void SetAsset(const FAssetData& AssetData) const;
};
