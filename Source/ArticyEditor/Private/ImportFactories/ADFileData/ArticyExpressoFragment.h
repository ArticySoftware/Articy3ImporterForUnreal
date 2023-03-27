//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyExpressoFragment.generated.h"

USTRUCT()
struct ARTICYEDITOR_API FArticyExpressoFragment
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Script")
	FString OriginalFragment = "";
	UPROPERTY(VisibleAnywhere, Category = "Script")
	FString ParsedFragment = "";
	UPROPERTY(VisibleAnywhere, Category = "Script")
	bool bIsInstruction = false;

	// TSet mandatory
	bool operator==(const FArticyExpressoFragment& Other) const
	{
		return bIsInstruction == Other.bIsInstruction && OriginalFragment == Other.OriginalFragment;
	}
};

// TSet mandatory
inline uint32 GetTypeHash(const FArticyExpressoFragment& A)
{
	return GetTypeHash(A.OriginalFragment) ^ GetTypeHash(A.bIsInstruction);
}
