//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArticyBaseTypes.h"
#include "Engine/Engine.h"
#include <Engine/World.h>
#include "Interfaces/ArticyReflectable.h"

#include "ArticyBaseObject.generated.h"

class UArticyPrimitive;
/**
 * Base class for all articy objects.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyBaseObject : public UDataAsset, public IArticyReflectable
{
	GENERATED_BODY()

public:

	/** Returns true if deserialization was already performed for this object. */
	bool WasLoaded() const { return bWasDeserialized; }
	/** For ArticyImporter internal use only: initializes the bWasDeserialized variable. */
	void Initialize() { bWasDeserialized = true; }

	UWorld* GetWorld() const override { return GEngine->GetWorldFromContextObjectChecked(GetOuter()); }

	UArticyPrimitive* GetSubobject(FArticyId Id) const;

protected:
	virtual void InitFromJson(TSharedPtr<FJsonValue> Json) { }

	template<typename Type, typename PropType>
	friend struct ArticyObjectTypeInfo;
	void AddSubobject(UArticyPrimitive* Obj);

	/** Objects that are dynamically created for this object during import (like Pins, Connections ecc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Articy")
	TMap<FArticyId, UArticyPrimitive*> Subobjects;

private:
	/** Initialized with false, changed to true by InitFromJson (and later by deserialization). */
	UPROPERTY(VisibleAnywhere, Category = "Articy")
	bool bWasDeserialized = false;
};

/** Base class for all Feature classes. */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyBaseFeature : public UArticyBaseObject
{
	GENERATED_BODY()

};
