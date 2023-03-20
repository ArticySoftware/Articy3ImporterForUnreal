//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ADHierarchy.generated.h"

UCLASS(BlueprintType)
class UADIHierarchyObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	FString Id;
	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	FString TechnicalName;
	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	FString Type;

	UPROPERTY(VisibleAnywhere, Category="HierarchyObject")
	TArray<UADIHierarchyObject*> Children;
};

USTRUCT()
struct ARTICYEDITOR_API FADHierarchy
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Hierarchy")
	UADIHierarchyObject* RootObject = nullptr;
};
