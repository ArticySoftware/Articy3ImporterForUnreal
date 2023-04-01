//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once
#include "CoreMinimal.h"
#include "ArticyBaseTypes.h"
#include "ArticyIdArray.generated.h"

USTRUCT()
struct FArticyIdArray
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FArticyId> Values;
};