//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once
#include "LocalizedLanguage.generated.h"

USTRUCT()
struct FADILocalizedLanguage
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Localization")
	FString CultureName;
	UPROPERTY(VisibleAnywhere, Category="Localization")
	int ArticyLanguageId;
	UPROPERTY(VisibleAnywhere, Category="Localization")
	FString LanguageName;

	void ImportFromJson(const TSharedPtr<FJsonObject> Json);
};