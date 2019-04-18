//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include <sstream>
#include "ArticyRef.h"
#include "ArticyBaseObject.h"
#include "ArticyBaseTypes.h"

#include "ArticyPrimitive.generated.h"

/**
 * A more lightweight base class for objects imported from articy.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyPrimitive : public UArticyBaseObject
{
	GENERATED_BODY()

public:
	FArticyId GetId() const { return Id; }
	uint32 GetCloneId() const { return CloneId; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FArticyId Id;

	friend struct FArticyClonableObject;
	/** The ID of this instance (0 is the original object). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CloneId = 0;

protected:
	/** Used internally by ArticyImporter. */
	void InitFromJson(TSharedPtr<FJsonValue> Json) override
	{
		UArticyBaseObject::InitFromJson(Json);

		if(!Json.IsValid() || Json->Type != EJson::Object)
			return;

		auto obj = Json->AsObject();
		if(!ensure(obj.IsValid()))
			return;

		JSON_TRY_HEX_ID(obj, Id);
	}

	friend UArticyPrimitive* FArticyRef::GetReference();

	static UArticyPrimitive* FindAsset(const FArticyId& Id);

private:
	mutable FString Path = "";
};
