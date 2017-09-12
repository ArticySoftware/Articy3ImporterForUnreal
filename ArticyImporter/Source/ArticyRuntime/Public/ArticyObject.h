//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "ArticyPrimitive.h"
#include "ArticyBaseTypes.h"
#include "ArticyRef.h"

#include "ArticyObject.generated.h"

/**
 * Base UCLASS for all articy objects.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyObject : public UArticyPrimitive
{
	GENERATED_BODY()

public:
	FName GetTechnicalName() const;

	UArticyObject* GetParent() const;
	TArray<UArticyObject*> GetChildren() const;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FArticyId Parent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString TechnicalName;

	/** Used internally by ArticyImporter. */
	void InitFromJson(TSharedPtr<FJsonValue> Json) override;
};
