//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once
#include "FADIFileHash.generated.h"

// @Alewinn ; split out of ImportData file to
// avoid diamond paradox/circular reference.
USTRUCT()
struct FADIFileHash
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Hash")
	FString FileName;
	UPROPERTY(VisibleAnywhere, Category="Hash")
	FString Hash;

	void ImportFromJson(const TSharedPtr<FJsonObject> Json);
};