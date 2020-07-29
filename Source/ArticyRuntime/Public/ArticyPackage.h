#pragma once

#include "CoreMinimal.h"
#include "ArticyObject.h"
#include "UObject/UObjectHash.h"
#include "ArticyPackage.generated.h"


UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyPackage : public UDataAsset
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Articy")
	TArray<UArticyObject*> Assets;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Articy")
	TMap<FName, TSoftObjectPtr<UArticyObject>> AssetsByTechnicalName;
public: 

	void AddAsset(UArticyObject* ArticyObject);

	UFUNCTION()
	const int AssetNum()const;

	UFUNCTION()
	void Clear();

	UFUNCTION()
	const TArray<UArticyObject*> GetAssets();

	UFUNCTION()
	const TMap<FName, TSoftObjectPtr<UArticyObject>> GetAssetsDict();

	UFUNCTION()
	const bool IsAssetContained(const UArticyObject* ArticyObject) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Package")
	FString Name;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Package")
	FString Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Package")
	bool bIsDefaultPackage = false;

private:
	// used to determine which objects are still parented to this package, which may include outdated articy objects that have to be deleted
	TArray<UObject*> GetInnerObjects() const;
};

inline const int UArticyPackage::AssetNum()const
{
	return Assets.Num();
}

inline void UArticyPackage::Clear()
{
	Assets.Empty();
	AssetsByTechnicalName.Empty();
}

inline const TArray<UArticyObject*> UArticyPackage::GetAssets()
{
	return Assets;
}

inline const TMap<FName, TSoftObjectPtr<UArticyObject>> UArticyPackage::GetAssetsDict()
{
	return AssetsByTechnicalName;
}

inline const bool UArticyPackage::IsAssetContained(const UArticyObject* ArticyObject) const
{
	if(!ArticyObject)
	{
		return false;
	}
	
	return AssetsByTechnicalName.Contains(ArticyObject->GetTechnicalName());
}

inline TArray<UObject*> UArticyPackage::GetInnerObjects() const
{
	TArray<UObject*> OutObjects;
	GetObjectsWithOuter(this, OutObjects, false);

	return OutObjects;
}

inline void UArticyPackage::AddAsset(UArticyObject* ArticyObject)
{
	const FName technicalName = ArticyObject->GetTechnicalName();

	if (!AssetsByTechnicalName.Contains(technicalName))
	{
		AssetsByTechnicalName.Add(technicalName, ArticyObject);
		Assets.Add(ArticyObject);
	}
}
