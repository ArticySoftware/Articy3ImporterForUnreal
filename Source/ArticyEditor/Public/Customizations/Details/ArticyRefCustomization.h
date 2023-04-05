//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include <IPropertyTypeCustomization.h>
#include "ArticyRef.h"
#include "Slate/SArticyIdProperty.h"
#include "Slate/SArticyRefProperty.h"
#include "ArticyObject.h"

// reference: color struct customization
class FArticyRefCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	static FArticyRef* RetrieveArticyRef(IPropertyHandle* ArticyIdHandle);
	static FArticyId GetIdFromValueString(FString SourceString);

private:
	TSharedPtr<IPropertyHandle> ArticyRefPropertyHandle;
	TSharedPtr<SWidget> ArticyRefPropertyWidget;

private:
	UClass* GetClassRestriction() const;
	bool IsExactClass() const;
	bool IsReadOnly() const;
	bool HasClassRestrictionMetaData() const;
	bool HasExactClassMetaData() const;

	FArticyRef GetArticyRef() const;
	void OnArticyRefChanged(const FArticyRef& NewArticyRef) const;
};
