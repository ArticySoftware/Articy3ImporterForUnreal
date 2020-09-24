//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyPrimitive.h"
#include "ArticyBaseTypes.h"
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

	UFUNCTION(BlueprintCallable, Category = "Articy")
	UArticyObject* GetParent() const;
	TArray<TWeakObjectPtr<UArticyObject>> GetChildren() const;
	
	FArticyId GetParentID() const;
	/** Includes all children IDs regardless of type (including pins etc.) */
	TArray<FArticyId> GetChildrenIDs() const;

#if WITH_EDITOR
	/** Includes all children IDs that map to articy objects (excluding pins etc.) */
	TArray<FArticyId> GetArticyObjectChildrenIDs() const;
	
	/** Find Asset fast maintains a transient database of all articy objects */
	static UArticyObject* FindAsset(const FArticyId& Id);
	static UArticyObject* FindAsset(const FString& TechnicalName);// MM_CHANGE

private:
	static TSet<TWeakObjectPtr<class UArticyPackage>> CachedPackages;
	static TMap<FArticyId, TWeakObjectPtr<UArticyObject>> ArticyIdCache;
	static TMap<FName, TWeakObjectPtr<UArticyObject>> ArticyNameCache;
#endif

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Articy")
	FArticyId Parent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Articy")
	TArray<FArticyId> Children;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Articy")
	FString TechnicalName;

	/** Used internally by ArticyImporter. */
	void InitFromJson(TSharedPtr<FJsonValue> Json) override;

private:

	mutable TArray<TWeakObjectPtr<UArticyObject>> CachedChildren;
};
