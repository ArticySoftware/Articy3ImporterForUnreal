//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "ArticyBaseTypes.h"
/**
 * 
 */

class UArticyObject;

namespace UserInterfaceHelperFunctions
{
	enum EImageSize
	{
		Small,
		Medium,
		Large
	};
	
	const FSlateBrush* GetArticyTypeImage(const UArticyObject* ArticyObject, EImageSize Size);

	UTexture2D* GetDisplayImage(const UArticyObject* ArticyObject);
	bool RetrievePreviewImage(const UArticyObject* ArticyObject, FSlateBrush& OutSlateBrush);
	bool RetrieveSpeakerPreviewImage(const UArticyObject* ArticyObject, FSlateBrush& OutSlateBrush);


	const FArticyId* GetTargetID(const UArticyObject* ArticyObject);
	const FString GetDisplayName(const UArticyObject* ArticyObject);
	const FLinearColor GetColor(const UArticyObject* ArticyObject);
	const bool ShowObjectInArticy(const UArticyObject* ArticyObject);
	const bool ShowObjectInArticy(const FArticyId ArticyId);

	
};
