//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyDatabase.h"
#include "ArticyBaseTypes.h"
#include "ArticyHelpers.h"
#include "ArticyGlobalVariables.h"
#include "ArticyPluginSettings.h"
#include "ArticyExpressoScripts.h"

FArticyShadowableObject::FArticyShadowableObject(UArticyPrimitive* Object)
{
	ShadowCopies.Add(FArticyObjectShadow(0, Object));
}

UArticyPrimitive* FArticyShadowableObject::Get(const IShadowStateManager* ShadowManager, bool bForceUnshadowed) const
{
	if (bForceUnshadowed)
		return ShadowCopies[0].Object;

	const auto ShadowLvl = ShadowManager->GetShadowLevel();
	FArticyObjectShadow* info = ShadowCopies.FindByPredicate([&](const FArticyObjectShadow& item)
	{
		return item.ShadowLevel == ShadowLvl;
	});

	if(info)
		return info->Object;

	if(!ensureMsgf(ShadowLvl > 0 && ShadowCopies.Num() > 0, TEXT("Cannot get shadow level %d of FArticyShadowableObject!"), ShadowLvl))
		return nullptr;

	//create a new shadow copy
	auto mostRecentShadow = ShadowCopies.Last().Object;
	auto obj = DuplicateObject(mostRecentShadow, mostRecentShadow);
	ShadowCopies.Add(FArticyObjectShadow(ShadowLvl, obj));
	const_cast<IShadowStateManager*>(ShadowManager)->RegisterOnPopState([=]
	{
		//when the state is popped, remove the shadow copy again
		//it is destroyed automatically, unless there is an owning reference to it
		ensure(ShadowCopies.Last().ShadowLevel == ShadowLvl);
		ShadowCopies.RemoveAt(ShadowCopies.Num() - 1);
	});

	//return the new shadow copy
	return obj;
}

FArticyClonableObject::FArticyClonableObject(UArticyPrimitive* BaseObject)
{
	AddClone(BaseObject, 0);
}

UArticyPrimitive* FArticyClonableObject::Get(const IShadowStateManager* ShadowManager, int32 CloneId, bool bForceUnshadowed) const
{
	auto info = Clones.Find(CloneId);
	return info ? info->Get(ShadowManager, bForceUnshadowed) : nullptr;
}

UArticyPrimitive* FArticyClonableObject::Clone(const IShadowStateManager* ShadowManager, int32 CloneId, bool bFailIfExists)
{
	auto clone = Get(ShadowManager, CloneId);

	if(bFailIfExists && clone)
		return nullptr;

	if(!clone)
	{
		//get the original object (clone 0)
		auto original = Get(ShadowManager);
		if(ensure(original))
		{
			//create the clone
			clone = DuplicateObject(original, original);
			AddClone(clone, CloneId);
		}
	}

	return clone;
}

void FArticyClonableObject::AddClone(UArticyPrimitive* Clone, int32 CloneId)
{
	if(!ensure(Clone))
		return;

	if(CloneId == -1)
	{
		//find the first free clone id
		for(int i = 0; i != -1; ++i)
		{
			if(!Clones.Find(i))
			{
				CloneId = i;
				break;
			}
		}
	}

	Clone->CloneId = CloneId;
	Clones.Add(CloneId, FArticyShadowableObject{Clone});
}

//---------------------------------------------------------------------------//

void UArticyDatabase::Init()
{
	LoadAllPackages(true);
}

UArticyDatabase* UArticyDatabase::Get(const UObject* WorldContext)
{
	bool bKeepBetweenWorlds = UArticyPluginSettings::Get()->bKeepDatabaseBetweenWorlds;

	if(bKeepBetweenWorlds && PersistentClone.IsValid())
		return PersistentClone.Get();

	//remove all clones who's world died (world == nullptr)
	Clones.Remove(nullptr);

	auto world = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	if(!ensureMsgf(world, TEXT("Could not get world from WorldContext %s"), WorldContext ? *WorldContext->GetName() : TEXT("NULL")))
		return nullptr;

	//find either the persistent clone or the clone that belongs to the world of the passed in context object
	auto& clone = bKeepBetweenWorlds ? PersistentClone : Clones.FindOrAdd(world);

	if(!clone.IsValid())
	{
		//clone not valid, create a new one
		UE_LOG(LogTemp, Warning, TEXT("Cloning ArticyDatabase."))

		//get the original asset to clone from
		auto asset = GetOriginal();
		if(!asset)
			return nullptr;

		//duplicate the original asset
		if(bKeepBetweenWorlds)
		{
			clone = DuplicateObject(asset, world->GetGameInstance());
#if !WITH_EDITOR
			clone->AddToRoot();
#endif
		}
		else
		{
			clone = DuplicateObject(asset, world);
		}

		//make the clone load its default packages
		if(clone.IsValid())
			clone->Init();
	}

	return clone.Get();
}

void UArticyDatabase::UnloadDatabase()
{
	bool bKeepBetweenWorlds = UArticyPluginSettings::Get()->bKeepDatabaseBetweenWorlds;

	TWeakObjectPtr<UArticyDatabase>* dbPtr = nullptr;
	if (bKeepBetweenWorlds)
		dbPtr = &PersistentClone;
	else
		dbPtr = &Clones.FindOrAdd(GetWorld());

	if (dbPtr && dbPtr->IsValid())
	{
		(*dbPtr)->RemoveFromRoot();
		(*dbPtr)->ConditionalBeginDestroy();
		*dbPtr = NULL;
	}
}

void UArticyDatabase::SetDefaultUserMethodsProvider(UObject * MethodProvider)
{
	GetExpressoInstance()->DefaultUserMethodsProvider = MethodProvider;
}

UArticyGlobalVariables* UArticyDatabase::GetGVs() const
{
	return UArticyGlobalVariables::GetDefault(this);
}

TArray<FString> UArticyDatabase::GetImportedPackageNames() const
{
	TArray<FString> outNames;
	ImportedPackages.GenerateKeyArray(outNames);
	return outNames;
}

bool UArticyDatabase::IsPackageDefaultPackage(FString PackageName)
{
	if(ImportedPackages.Contains(PackageName))
	{
		const FArticyPackage& Package = ImportedPackages[PackageName];
		return Package.bIsDefaultPackage;
	}

	return false;
}

UWorld* UArticyDatabase::GetWorld() const
{
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

void UArticyDatabase::LoadAllObjects()
{
	GetOriginal(true);
}

void UArticyDatabase::SetLoadedPackages(const TArray<FArticyPackage> Packages)
{
	ImportedPackages.Reset();
	UnloadAllPackages();

	for(auto pkg : Packages)
		ImportedPackages.Add(pkg.Name, pkg);
}

//---------------------------------------------------------------------------//

void UArticyDatabase::LoadDefaultPackages()
{
	LoadAllPackages(true);
}

void UArticyDatabase::LoadAllPackages(bool bDefaultOnly)
{
	const UArticyPluginSettings* settings = GetDefault<UArticyPluginSettings>();
	for(const auto pack : ImportedPackages)
	{
		if(!bDefaultOnly || pack.Value.bIsDefaultPackage
#if WITH_EDITOR
			//TODO add "or is edit mode"
#endif
		)
			LoadPackage(pack.Key);
	}
}

void UArticyDatabase::LoadPackage(FString PackageName)
{
	if(LoadedPackages.Contains(PackageName))
	{
		UE_LOG(LogArticyRuntime, Log, TEXT("Package %s already loaded."), *PackageName);
		return;
	}

	FArticyPackage* package = ImportedPackages.Find(PackageName);
	if(!package)
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("Failed to find Package %s in imported packages!"), *PackageName);
		return;
	}

	/*auto fileName = PackageName.Replace(TEXT(" "), TEXT("_"));
	auto pkgFile = Cast<UPackage>(::LoadPackage(nullptr, *fileName, 0));
	if(!pkgFile)
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("Failed to load package file %s from filesystem!"), *fileName);
		return;
	}

	//load the package, to make sure all the containging objects are available
	pkgFile->FullyLoad();*/

	for(const auto obj : package->Objects)
	{
		auto primitiveClone = DuplicateObject(Cast<UArticyPrimitive>(obj), this);
		if(!primitiveClone)
			continue;

		auto id = primitiveClone->GetId();
		ensureMsgf(!LoadedObjectsById.Contains(id), TEXT("Object with id [%d,%d] already in list!"), id.High, id.Low);

		auto shared = MakeShared<FArticyClonableObject>(primitiveClone);
		LoadedObjectsById.Add(id, shared);
		ArticyObjects.Add(primitiveClone);

		auto objectClone = Cast<UArticyObject>(primitiveClone);
		if(objectClone)
			LoadedObjectsByName.FindOrAdd(objectClone->GetTechnicalName()).Objects.Add(shared);
	}

	LoadedPackages.Add(PackageName);
	UE_LOG(LogArticyRuntime, Log, TEXT("Package %s loaded successfully."), *PackageName);
}

void UArticyDatabase::UnloadAllPackages()
{
	LoadedPackages.Reset();
	LoadedObjectsById.Reset();
	LoadedObjectsByName.Reset();
	ArticyObjects.Reset();
}

void UArticyDatabase::SetExpressoScriptsClass(TSubclassOf<UArticyExpressoScripts> NewClass)
{
	ExpressoScriptsClass = NewClass;
}

const UArticyDatabase* UArticyDatabase::GetOriginal(bool bLoadAllPackages)
{
	static TWeakObjectPtr<UArticyDatabase> Asset = nullptr;

	if(!Asset.IsValid())
	{
		//create a clone of the database
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(StaticClass()->GetFName(), AssetData, true);

		if(ensureMsgf(AssetData.Num() != 0, TEXT("Could not find original asset of ArticyDraftDatabase!")))
		{
			if(AssetData.Num() > 1)
			UE_LOG(LogTemp, Warning, TEXT("More than one ArticyDraftDatabase was found, this is not supported! The first one will be selected."));

			Asset = Cast<UArticyDatabase>(AssetData[0].GetAsset());

			if(bLoadAllPackages && Asset.IsValid())
				Asset.Get()->LoadAllPackages();
		}
	}

	return Asset.Get();
}

//---------------------------------------------------------------------------//

UArticyPrimitive* UArticyDatabase::GetObject(FArticyId Id, int32 CloneId, TSubclassOf<class UArticyObject> CastTo) const
{
	return GetObjectInternal(Id, CloneId);
}

UArticyPrimitive* UArticyDatabase::GetObjectUnshadowed(FArticyId Id, int32 CloneId) const
{
	return GetObjectInternal(Id, CloneId, true);
}

UArticyPrimitive* UArticyDatabase::GetObjectInternal(FArticyId Id, int32 CloneId, bool bForceUnshadowed) const
{
	const TSharedPtr<FArticyClonableObject>* info = LoadedObjectsById.Find(Id);
	return info && info->IsValid() ? info->Get()->Get(this, CloneId, bForceUnshadowed) : nullptr;
}

UArticyObject* UArticyDatabase::GetObjectByName(FName TechnicalName, int32 CloneId, TSubclassOf<class UArticyObject> CastTo) const
{
	auto arr = LoadedObjectsByName.Find(TechnicalName);
	if(!arr || arr->Objects.Num() <= 0)
		return nullptr;

	auto info = arr->Objects[0];
	return info.IsValid() ? Cast<UArticyObject>(info.Pin()->Get(this, CloneId)) : nullptr;
}

TArray<UArticyObject*> UArticyDatabase::GetObjects(FName TechnicalName, int32 CloneId, TSubclassOf<class UArticyObject> CastTo) const
{
	return GetObjects<UArticyObject>(TechnicalName, CloneId);
}

TArray<UArticyObject*> UArticyDatabase::GetObjectsOfClass(TSubclassOf<class UArticyObject> Type, int32 CloneId) const
{
	TArray<UArticyObject*> arr;
	for (auto obj : ArticyObjects)
		if (obj->GetCloneId() == CloneId && obj->IsA(Type))
			arr.Add(Cast<UArticyObject>(obj));

	return arr;
}

TArray<UArticyPrimitive*> UArticyDatabase::GetAllObjects() const
{
	return ArticyObjects;
}

//---------------------------------------------------------------------------//

UArticyPrimitive* UArticyDatabase::CloneFrom(FArticyId Id, int32 NewCloneId, TSubclassOf<class UArticyObject> CastTo)
{
	auto info = LoadedObjectsById.Find(Id);
	return info && info->IsValid() ? info->Get()->Clone(this, NewCloneId, true) : nullptr;
}

UArticyObject* UArticyDatabase::CloneFromByName(FName TechnicalName, int32 NewCloneId, TSubclassOf<class UArticyObject> CastTo)
{
	auto arr = LoadedObjectsByName.Find(TechnicalName);
	if(!arr || arr->Objects.Num() <= 0)
		return nullptr;

	auto info = arr->Objects[0];
	return info.IsValid() ? Cast<UArticyObject>(info.Pin()->Clone(this, NewCloneId, true)) : nullptr;
}

//---------------------------------------------------------------------------//

UArticyPrimitive* UArticyDatabase::GetOrClone(FArticyId Id, int32 NewCloneId)
{
	auto info = LoadedObjectsById.Find(Id);
	return info && info->IsValid() ? info->Get()->Clone(this, NewCloneId, false) : nullptr;
}

UArticyObject* UArticyDatabase::GetOrCloneByName(const FName& TechnicalName, int32 NewCloneId)
{
	auto arr = LoadedObjectsByName.Find(TechnicalName);
	if(!arr || arr->Objects.Num() <= 0)
		return nullptr;

	auto info = arr->Objects[0];
	return info.IsValid() ? Cast<UArticyObject>(info.Pin()->Clone(this, NewCloneId, false)) : nullptr;
}

UArticyExpressoScripts* UArticyDatabase::GetExpressoInstance() const
{
	if(!CachedExpressoScripts)
	{
		if(ensure(ExpressoScriptsClass))
		{
			CachedExpressoScripts = NewObject<UArticyExpressoScripts>(const_cast<UArticyDatabase*>(this), ExpressoScriptsClass);
			CachedExpressoScripts->Init(const_cast<UArticyDatabase*>(this));
		}
	}

	return CachedExpressoScripts;
}

UArticyDatabase* UArticyDatabase::GetMutableOriginal()
{
	static TWeakObjectPtr<UArticyDatabase> Asset = nullptr;

	if (!Asset.IsValid())
	{
		//create a clone of the database
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(StaticClass()->GetFName(), AssetData, true);

		if (ensureMsgf(AssetData.Num() != 0, TEXT("Could not find original asset of ArticyDraftDatabase!")))
		{
			if (AssetData.Num() > 1)
				UE_LOG(LogTemp, Warning, TEXT("More than one ArticyDraftDatabase was found, this is not supported! The first one will be selected."));

			Asset = Cast<UArticyDatabase>(AssetData[0].GetAsset());
		}
	}

	return Asset.Get();
}

void UArticyDatabase::ChangePackageDefault(FName PackageName, bool bIsDefaultPackage)
{
	if(ImportedPackages.Contains(PackageName.ToString()))
	{
		FArticyPackage* Package = ImportedPackages.Find(PackageName.ToString());
		Package->bIsDefaultPackage = bIsDefaultPackage;
	}
}

TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<UArticyDatabase>> UArticyDatabase::Clones;
TWeakObjectPtr<UArticyDatabase> UArticyDatabase::PersistentClone;