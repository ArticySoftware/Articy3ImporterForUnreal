//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Engine/DataAsset.h"
#include "ArticyBaseObject.h"
#include "Private/ShadowStateManager.h"
#include "AssetRegistryModule.h"
#include "ArticyObject.h"
#include "Serialization/Archive.h"

#include "ArticyDatabase.generated.h"

class UArticyExpressoScripts;
struct FArticyId;
struct FArticyPackage;
class UArticyGlobalVariables;

USTRUCT(BlueprintType)
struct FArticyObjectShadow
{
	GENERATED_BODY()
	
	FArticyObjectShadow() {}
	FArticyObjectShadow(uint32 shadowLevel, UArticyPrimitive* object) : ShadowLevel(shadowLevel), Object(object) {}

public:
	UPROPERTY()
	uint32 ShadowLevel;
	UPROPERTY()
	UArticyPrimitive* Object;
};

USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyShadowableObject
{
	GENERATED_BODY()

public:
	FArticyShadowableObject() = default;
	/**
	 * This constructor writes the Object to Shadows[0].
	 */
	explicit FArticyShadowableObject(UArticyPrimitive* Object);

	/**
	 * Returns the requested shadow.
	 */
	UArticyPrimitive* Get(const IShadowStateManager* ShadowManager, bool ForceUnshadowed = false) const;

private:

	/**
	 * The original [0] object and its shadows.
	 * Shadows are guaranteed to be stored in ascending order, but
	 * the array index is not the shadow level (except for 0)!
	 */
	UPROPERTY(VisibleAnywhere)
	mutable TArray<FArticyObjectShadow> ShadowCopies;
};

/**
 * Contains a reference to a UArticyObject, and to its clones if any.
 */
USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyClonableObject
{
	GENERATED_BODY()

public:
	FArticyClonableObject() = default;
	FArticyClonableObject(UArticyPrimitive* BaseObject);

	//========================================//

	/**
	 * Get the clone of this object with a certain CloneId.
	 * Returns nullptr if the clone does not exist.
	 */
	UArticyPrimitive* Get(const IShadowStateManager* ShadowManager, int32 CloneId = 0, bool bForceUnshadowed = false) const;
	/**
	 * Clone the original object and assign it the id CloneId.
	 * If bFailIfExists is true, nullptr is returned if the clone already exists.
	 * Otherwise, the existing clone is returned.
	 */
	UArticyPrimitive* Clone(const IShadowStateManager* ShadowManager, int32 CloneId, bool bFailIfExists = true);

private:

	/**
	 * The copied instances of the same object.
	 * Clones[0] is the one that is created at startup from the object assets.
	 */
	UPROPERTY(VisibleAnywhere)
	TMap<int32, FArticyShadowableObject> Clones;

	/** Adds a clone to the Clones map. */
	void AddClone(UArticyPrimitive* Clone, int32 CloneId);
};

/**
 * Contains an array of FArticyDatabaseObjects.
 */
USTRUCT(BlueprintType)
struct ARTICYRUNTIME_API FArticyDatabaseObjectArray
{
	GENERATED_BODY()

public:
	TArray<TWeakPtr<FArticyClonableObject>> Objects;
};

/**
 * The database is used for accessing or cloning any articy object.
 */
UCLASS(Config=Game)
class ARTICYRUNTIME_API UArticyDatabase : public UDataAsset, public IShadowStateManager
{
	GENERATED_BODY()

public:

	void Init();

	/** Get the static instance of the database. */
	static UArticyDatabase* Get(const UObject* WorldContext);

	/** Get the current GVs instance. */
	virtual UArticyGlobalVariables* GetGVs() const;

	/** Unloads the database, which causes that all changes get removed.*/
	UFUNCTION(BlueprintCallable)
	void UnloadDatabase();

	/** 
	 * Returns true if the database is in shadow state.
	 * Can be used in script methods to determine if the function is called during
	 * a flow player branch calculation.
	 */
	UFUNCTION(BlueprintPure, meta=(DisplayName="Is in shadow state?"), Category = "Script Methods")
	bool IsInShadowState() const { return GetShadowLevel()  > 0; }

	UWorld* GetWorld() const override;

	//---------------------------------------------------------------------------//

	/**
	 * Load all objects imported from articy, so the UE4 packages are 'fully loaded'.
	 * Used internally, so ArticyRef can find the referenced object.
	 */
	static void LoadAllObjects();

	virtual void SetLoadedPackages(const TArray<FArticyPackage> Packages);

	/** Load all packages which have the bIsDefaultPackage flag set to true. */
	virtual void LoadDefaultPackages();

	/** Load all imported packages. */
	virtual void LoadAllPackages(bool bDefaultOnly = false);

	/** Load a package of a given name. */
	UFUNCTION(BlueprintCallable, Category = "Packages")
	virtual void LoadPackage(FString PackageName);

	//---------------------------------------------------------------------------//

	/**
	 * Get an object by its Id.
	 * If a CloneId other than 0 is provided, a copy of the object with this index must exist.
	 * Otherwise a null-pointer is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	UArticyPrimitive* GetObject(FArticyId Id, int32 CloneId = 0) const;
	template<typename T>
	T* GetObject(FArticyId Id, int32 CloneId = 0) const { return Cast<T>(GetObject(Id, CloneId)); }

	UArticyPrimitive* GetObjectUnshadowed(FArticyId Id, int32 CloneId = 0) const;

	/**
	 * Get an object by its TechnicalName.
	 * If a CloneId other than 0 is provided, a copy of the object with this index must exist,
	 * otherwise a null-pointer is returned.
	 * Note that the TechnicalName is not unique! This will take the first matching object.
	 */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	UArticyObject* GetObjectByName(FName TechnicalName, int32 CloneId = 0) const;
	template<typename T>
	T* GetObjectByName(FName TechnicalName, int32 CloneId = 0) const { return Cast<T>(GetObjectByName(TechnicalName, CloneId)); }

	/**
	 * Get all objects with a given TechnicalName.
	 * If a CloneId other than 0 is provided, copies of the objects with this index must exist,
	 * otherwise null-pointers are returned instead.
	 * Note: this allocates a new TArray, use the other variant if you already have an array
	 * to fill the objects with!
	 */
	template<typename T>
	TArray<T*> GetObjects(FName TechnicalName, int32 CloneId = 0) const;
	/**
	 * Get all objects with a given TechnicalName.
	 * If a CloneId other than 0 is provided, copies of the objects with this index must exist,
	 * otherwise null-pointers are returned instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	TArray<UArticyObject*> GetObjects(FName TechnicalName, int32 CloneId = 0) const;

	/**
	* Get all objects with a given Type.
	* If a CloneId other than 0 is provided, copies of the objects with this index must exist,
	* otherwise they will be not added to the result.
	*/
	template<typename T>
	TArray<T*> GetObjectsByType(int32 CloneId = 0) const;

	/**
	* Get all objects with a given Type.
	* If a CloneId other than 0 is provided, copies of the objects with this index must exist,
	* otherwise they will be not added to the result.
	*/
	UFUNCTION(BlueprintCallable, Category = "Objects")
	TArray<UArticyObject*> GetObjectsByType(TSubclassOf<class UArticyObject> Type, int32 CloneId = 0) const;

	/**
	* Get all objects.
	*/
	UFUNCTION(BlueprintCallable, Category = "Objects")
	TArray<UArticyPrimitive*> GetAllObjects() const;

	/**
	 * Get all objects with a given TechnicalName.
	 * If a CloneId other than 0 is provided, copies of the objects with this index must exist,
	 * otherwise null-pointers are returned instead.
	 */
	template<typename T = UArticyObject>
	void GetObjects(TArray<T*>& Array, FName TechnicalName, int32 CloneId = 0) const;
	
	//---------------------------------------------------------------------------//

	/**
	 * Clone an existing object, and assign the NewCloneId to it.
	 * If the clone already exists, nullptr is returned!
	 * If NewCloneId is -1, the next free clone Id will be used.
	 */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	UArticyPrimitive* CloneFrom(FArticyId Id, int32 NewCloneId = -1);
	template<typename T>
	T* CloneFrom(FArticyId Id, int32 NewCloneId = -1) { return Cast<T>(CloneFrom(Id, NewCloneId)); }

	/**
	 * Clone an existing object, and assign the NewCloneId to it.
	 * If the clone already exists, nullptr is returned!
	 * If NewCloneId is -1, the next free clone Id will be used.
	 */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	UArticyObject* CloneFromByName(FName TechnicalName, int32 NewCloneId = -1);
	template<typename T>
	T* CloneFrom(FName TechnicalName, int32 NewCloneId = -1) { return Cast<T>(CloneFromByName(TechnicalName, NewCloneId)); }

	/**
	 * Clone an existing object, and assign the NewCloneId to it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	UArticyPrimitive* GetOrClone(FArticyId Id, int32 NewCloneId);
	template<typename T>
	T* GetOrClone(FArticyId Id, int32 NewCloneId) { return Cast<T>(GetOrClone(Id, NewCloneId)); }

	/**
	 * Clone an existing object, and assign the NewCloneId to it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Objects")
	UArticyObject* GetOrCloneByName(const FName& TechnicalName, int32 NewCloneId);
	template<typename T>
	T* GetOrClone(const FName& TechnicalName, int32 NewCloneId) { return Cast<T>(GetOrCloneByName(TechnicalName, NewCloneId)); }

	//---------------------------------------------------------------------------//

	/**
	 * Gets a UArticyExpressoScripts instance, which can be used to execute
	 * instructions or evaluate conditions.
	 */
	UArticyExpressoScripts* GetExpressoInstance() const;

protected:

	/** A list of all packages that were imported from articy:draft. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FString, FArticyPackage> ImportedPackages;

	UPROPERTY(VisibleAnywhere, transient)
	TArray<FString> LoadedPackages;

	TMap<FArticyId, TSharedPtr<FArticyClonableObject>> LoadedObjectsById;
	TMap<FName, FArticyDatabaseObjectArray> LoadedObjectsByName;
	/** Holds the references to all cloned articy objects to prevent them from being garbage collected. */
	UPROPERTY()
	TArray<UArticyPrimitive *> ArticyObjects;

	void UnloadAllPackages();

	UFUNCTION(BlueprintCallable, Category="Setup")
	void SetExpressoScriptsClass(TSubclassOf<UArticyExpressoScripts> NewClass);

private:

	static TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<UArticyDatabase>> Clones;
	static TWeakObjectPtr<UArticyDatabase> PersistentClone;

	UPROPERTY()
	mutable UArticyExpressoScripts* CachedExpressoScripts;

	/** An instance of this class will be used to execute script fragments. */
	UPROPERTY(Config, VisibleAnywhere)
	TSubclassOf<UArticyExpressoScripts> ExpressoScriptsClass;

	UArticyPrimitive* GetObjectInternal(FArticyId Id, int32 CloneId = 0, bool bForceUnshadowed = false) const;

	/** Get the original asset (on disk) of the database. */
	static const UArticyDatabase* GetOriginal(bool bLoadDefaultPackages = false);
};

template<typename T>
TArray<T*> UArticyDatabase::GetObjects(FName TechnicalName, int32 CloneId) const
{
	static TArray<T*> Array;
	Array.Reset();
	
	GetObjects(Array, TechnicalName, CloneId);

	return Array;
}

template<typename T>
TArray<T*> UArticyDatabase::GetObjectsByType(int32 CloneId) const
{
	TArray<T*> arr;
	for (auto obj : ArticyObjects)
		if (obj->GetCloneId() == CloneId && Cast<T>(obj))
			arr.Add(obj);

	return arr;
}

template<typename T>
void UArticyDatabase::GetObjects(TArray<T*>& Array, FName TechnicalName, int32 CloneId) const
{
	//find all objects with this name
	auto arr = LoadedObjectsByName.Find(TechnicalName);
	if(arr)
	{
		for(const auto obj : arr->Objects)
		{
			auto clone = Cast<T>(obj.Pin()->Clone(this, CloneId));
			if(clone)
				Array.Add(clone);
		}
	}
}
