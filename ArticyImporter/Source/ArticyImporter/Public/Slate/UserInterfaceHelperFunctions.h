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

	UTexture2D* GetDisplayImage(const UArticyObject* ArticyObject);

	const FString GetDisplayName(const UArticyObject* ArticyObject);


	
};
