//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Articy")
	FArticyId Id;

	// TODO k2 - changed to UArticyCloneableObject
	//friend struct FArticyClonableObject;
	friend struct FArticyObjectShadow;
	/** The ID of this instance (0 is the original object). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Articy")
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
	
private:
	mutable FString Path = "";
};
