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
	
	const ARTICYEDITOR_API FSlateBrush* GetArticyTypeImage(const UArticyObject* ArticyObject, EImageSize Size);

	ARTICYEDITOR_API UTexture2D* GetDisplayImage(const UArticyObject* ArticyObject);
	ARTICYEDITOR_API bool RetrievePreviewImage(const UArticyObject* ArticyObject, FSlateBrush& OutSlateBrush);
	ARTICYEDITOR_API bool RetrieveSpeakerPreviewImage(const UArticyObject* ArticyObject, FSlateBrush& OutSlateBrush);

	const ARTICYEDITOR_API FArticyId* GetTargetID(const UArticyObject* ArticyObject);
	const ARTICYEDITOR_API FString GetDisplayName(const UArticyObject* ArticyObject);
	const ARTICYEDITOR_API FLinearColor GetColor(const UArticyObject* ArticyObject);
	//const TArray< GetColor(const UArticyObject* ArticyObject);

	const ARTICYEDITOR_API bool ShowObjectInArticy(const UArticyObject* ArticyObject);
	const ARTICYEDITOR_API bool ShowObjectInArticy(const FArticyId ArticyId, bool bNewTab);
};
