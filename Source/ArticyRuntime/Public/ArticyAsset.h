//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyObject.h"
#include "FileMediaSource.h"
#include "Sound/SoundWave.h"
#include "ArticyAsset.generated.h"

UENUM(BlueprintType)
enum class EArticyAssetCategory : uint8
{
	None = 0,

	/** All image related asset types, also icons and svg. */
	Image = 1,
	/** All video related asset types*/
	Video = 2,
	/** All audio related asset types */
	Audio = 4,
	/** All documents, like txt and doc. */
	Document = 8,
	/** Everything else, like obj, blend, fbx etc. */
	Misc = 16,

	/** Defines a flag to describe all assets, used when declaring supported assets. */
	All = 0xFF,
};

/**
 * Base class for all imported assets.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyAsset : public UArticyObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Load Asset")
	UObject* LoadAsset() const;
	
	UFUNCTION(BlueprintCallable, Category="Load Asset")
	UTexture* LoadAsTexture() const;

	UFUNCTION(BlueprintCallable, Category="Load Asset")
	UTexture2D* LoadAsTexture2D() const;

	UFUNCTION(BlueprintCallable, Category="Load Asset")
	USoundWave* LoadAsSoundWave() const;

	UFUNCTION(BlueprintCallable, Category = "Load Asset")
	UFileMediaSource* LoadAsFileMediaSource() const;
	
	/** The relative path of the referenced asset. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Meta Data")
	FString AssetRef;
	/** The category of the referenced asset. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Meta Data")
	EArticyAssetCategory Category;

private:
	UPROPERTY(Transient, VisibleAnywhere, Category = "Articy")
	mutable TWeakObjectPtr<UObject> Asset = nullptr;
};
