//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

//#include "Engine.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ArticyBaseTypes.h"
#include "ArticyReflectable.h"
#include "Engine/Engine.h"
#include <Engine/World.h>

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FArticyId, UArticyPrimitive*> Subobjects;

private:
	/** Initialized with false, changed to true by InitFromJson (and later by deserialization). */
	UPROPERTY(VisibleAnywhere)
	bool bWasDeserialized = false;
};

/** Base class for all Feature classes. */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyBaseFeature : public UArticyBaseObject
{
	GENERATED_BODY()

};
