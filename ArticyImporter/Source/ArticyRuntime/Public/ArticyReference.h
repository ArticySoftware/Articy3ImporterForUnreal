//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyRef.h"
#include "Components/ActorComponent.h"
#include "ArticyReference.generated.h"


/**
* Component to hold a single ArticyRef.
* While you can use ArticyRef in your scripts directly, ArticyReference is most useful in generated code or as a simple way for designers to attach articy objects to actors
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARTICYRUNTIME_API UArticyReference : public UActorComponent
{
	GENERATED_BODY()

public:	
	UArticyReference();

	/** The ArticyRef, which this component holds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objects")
	FArticyRef Reference;

	/** Get a value indicating wether the id of the referenced object is valid */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	bool IsValid();
	/** Get the referenced object */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	UArticyPrimitive* GetObject(const UObject* WorldContext);
	/** Set the referenced object */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	void SetReference(UArticyPrimitive* Object);
	
};
