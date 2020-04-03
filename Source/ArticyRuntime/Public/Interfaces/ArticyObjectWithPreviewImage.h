//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObjectWith_Base.h"
#include "ArticyBuiltinTypes.h"
#include "ArticyObjectWithPreviewImage.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithPreviewImage : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'PreviewImage' implement this interface.
 */
class IArticyObjectWithPreviewImage : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithPreviewImage")
	virtual UArticyPreviewImage*& GetPreviewImage()
	{
		static const auto PropName = FName("PreviewImage");
		return GetProperty<UArticyPreviewImage*>(PropName);
	}

	virtual const UArticyPreviewImage* GetPreviewImage() const
	{
		return const_cast<IArticyObjectWithPreviewImage*>(this)->GetPreviewImage();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithPreviewImage")
	virtual UArticyPreviewImage*& SetPreviewImage(UArticyPreviewImage* PreviewImage)
	{
		return GetPreviewImage() = PreviewImage;
	}
};
