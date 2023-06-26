//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ArticyTexts.generated.h"

USTRUCT()
struct ARTICYEDITOR_API FArticyTextDef
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Text")
	FString Text = "";

	UPROPERTY(VisibleAnywhere, Category="Text")
	FString VOAsset = "";

	void ImportFromJson(const TSharedPtr<FJsonValue>& Json);
};

USTRUCT()
struct FArticyTexts
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Texts")
	TMap<FString, FArticyTextDef> Content;
	
	UPROPERTY(VisibleAnywhere, Category="Texts")
	FString Context = "";
	
	void ImportFromJson(const TSharedPtr<FJsonObject>& Json);
};
