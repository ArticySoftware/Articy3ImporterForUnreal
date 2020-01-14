//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "CoreMinimal.h"
#include "ArticyPrimitive.h"
#include "ArticyBaseTypes.h"
#include "ArticyRef.h"
#include "Dom/JsonValue.h"

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

	UFUNCTION(BlueprintCallable)
	UArticyObject* GetParent() const;
	TArray<TWeakObjectPtr<UArticyObject>> GetChildren() const;
	
	FArticyId GetParentID() const;
	TArray<FArticyId> GetChildrenIDs() const;
	
	static UArticyObject* FindAsset(const FArticyId& Id);
	static UArticyObject* FindAsset(const FString& TechnicalName);// MM_CHANGE

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FArticyId Parent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FArticyId> Children;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString TechnicalName;

	/** Used internally by ArticyImporter. */
	void InitFromJson(TSharedPtr<FJsonValue> Json) override;

private:

	mutable TArray<TWeakObjectPtr<UArticyObject>> CachedChildren;
};
