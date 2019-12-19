// Fill out your copyright notice in the Description page of Project Settings.

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
