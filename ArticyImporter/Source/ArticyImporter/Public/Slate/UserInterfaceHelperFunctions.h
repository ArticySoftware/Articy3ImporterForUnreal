//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
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



	const FString GetDisplayName(const UArticyObject* ArticyObject);

	const bool ShowObjectInArticy(const UArticyObject* ArticyObject);
	
};
