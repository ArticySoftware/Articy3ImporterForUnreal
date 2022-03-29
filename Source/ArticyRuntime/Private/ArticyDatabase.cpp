//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyDatabase.h"
#include "ArticyBaseTypes.h"
#include "ArticyGlobalVariables.h"
#include "ArticyPluginSettings.h"
#include "ArticyExpressoScripts.h"
#include "Misc/Paths.h"


UArticyObject* FArticyObjectShadow::GetObject()
{
	return Object;
}

FArticyShadowableObject::FArticyShadowableObject(UArticyObject* Object, int32 CloneId, UObject* Outer)
{
	ShadowCopies.Add(FArticyObjectShadow(0, Object, CloneId, Outer));
}

UArticyObject* FArticyShadowableObject::Get(const IShadowStateManager* ShadowManager, bool bForceUnshadowed) const
{
	if (bForceUnshadowed)
		return ShadowCopies[0].GetObject();

	const auto ShadowLvl = ShadowManager->GetShadowLevel();
	FArticyObjectShadow* info = ShadowCopies.FindByPredicate([&](const FArticyObjectShadow& item)
	{
		return item.ShadowLevel == ShadowLvl;
	});

	if(info)
		return info->GetObject();

	if(!ensureMsgf(ShadowLvl > 0 && ShadowCopies.Num() > 0, TEXT("Cannot get shadow level %d of FArticyShadowableObject!"), ShadowLvl))
		return nullptr;

	//create a new shadow copy
	auto& mostRecentShadow = ShadowCopies.Last();
	auto SourceObject = mostRecentShadow.GetObject();
	auto obj = DuplicateObject(SourceObject, SourceObject);
	ShadowCopies.Add(FArticyObjectShadow(ShadowLvl, obj, mostRecentShadow.GetCloneId()) );
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

UArticyObject* UArticyCloneableObject::Get(const IShadowStateManager* ShadowManager, int32 CloneId,
                                           bool bForceUnshadowed) const
{
	auto info = Clones.Find(CloneId);
	return info ? info->Get(ShadowManager, bForceUnshadowed) : nullptr;
}

UArticyObject* UArticyCloneableObject::Clone(const IShadowStateManager* ShadowManager, int32 CloneId,
                                             bool bFailIfExists)
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

void UArticyCloneableObject::AddClone(UArticyObject* Clone, int32 CloneId)
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

	Clones.Add(CloneId, FArticyShadowableObject{ Clone, CloneId });
}

//---------------------------------------------------------------------------//

void UArticyDatabase::Init()
{
	LoadDefaultPackages();
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
	GetExpressoInstance()->SetDefaultUserMethodsProvider(MethodProvider);
}

UArticyGlobalVariables* UArticyDatabase::GetGVs() const
{
	// If we have an active variable set, grab that set
	UArticyGlobalVariables* ActiveGlobals = GetExpressoInstance()->GetGV();
	if (ActiveGlobals)
		return ActiveGlobals;

	// Otherwise, return the default GVs
	return UArticyGlobalVariables::GetDefault(this);
}

UArticyGlobalVariables* UArticyDatabase::GetRuntimeGVs(UArticyAlternativeGlobalVariables* Asset) const
{
	// Find or create a clone of the global variables set associated with this "alternative" set
	return UArticyGlobalVariables::GetRuntimeClone(this, Asset);
}

TArray<FString> UArticyDatabase::GetImportedPackageNames() const
{
	TArray<FString> outNames;
	ImportedPackages.GenerateKeyArray(outNames);
	return outNames;
}

bool UArticyDatabase::IsPackageDefaultPackage(FString PackageName)
{
	if (ImportedPackages.Contains(PackageName))
	{
		const UArticyPackage* Package = ImportedPackages[PackageName];
		// in case the key exists but has no valid entry
		if (!Package)
		{
			return false;
		}
		
		return Package->bIsDefaultPackage;
	}

	return false;
}

void UArticyDatabase::LoadAllObjects()
{
	GetOriginal(true);
}

void UArticyDatabase::SetLoadedPackages(const TArray<UArticyPackage*> Packages)
{
	ImportedPackages.Reset();
	UnloadAllPackages();

	for (auto pkg : Packages)
	{
		ImportedPackages.Add(pkg->Name, pkg);
	}
}

//---------------------------------------------------------------------------//

void UArticyDatabase::LoadDefaultPackages()
{
	LoadAllPackages(true);
}

void UArticyDatabase::LoadAllPackages(bool bDefaultOnly)
{
	for (const auto &pack : ImportedPackages)
	{
		if(!bDefaultOnly || (pack.Value && pack.Value->bIsDefaultPackage)
#if WITH_EDITOR
			//TODO add "or is edit mode"
#endif
		)
			LoadPackage(pack.Key);
	}
}

void UArticyDatabase::LoadPackage(FString PackageName)
{	
	if (LoadedPackages.Contains(PackageName))
	{
		UE_LOG(LogArticyRuntime, Log, TEXT("Package %s already loaded."), *PackageName);
		return;
	}
	
	if(!ImportedPackages.Contains(PackageName) || ImportedPackages[PackageName] == nullptr)
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("Failed to find Package %s in imported packages!"), *PackageName);
		return;
	}
	
	UArticyPackage* Package = ImportedPackages[PackageName];

	/*auto fileName = PackageName.Replace(TEXT(" "), TEXT("_"));
	auto pkgFile = Cast<UPackage>(::LoadPackage(nullptr, *fileName, 0));
	if(!pkgFile)
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("Failed to load package file %s from filesystem!"), *fileName);
		return;
	}

	//load the package, to make sure all the contained objects are available
	pkgFile->FullyLoad();*/

	for (auto ArticyObject : Package->GetAssets())// MM_CHANGE
	{
		auto id = FArticyId(ArticyObject->GetId());

		if (!ensureMsgf(!LoadedObjectsById.Contains(id), TEXT("Object with id [%d,%d] already in list!"), id.High, id.Low))
			continue;

		auto CloneContainer = NewObject<UArticyCloneableObject>(this);
		UArticyObject* InitialClone = DuplicateObject<UArticyObject>(ArticyObject, this);
		CloneContainer->Init(InitialClone);
		
		LoadedObjectsById.Add(id, CloneContainer);

		if (!ArticyObject->GetTechnicalName().ToString().IsEmpty())
		{
			LoadedObjectsByName.FindOrAdd(ArticyObject->GetTechnicalName()).Objects.Add(CloneContainer);
		}
	}

	LoadedPackages.Add(PackageName);
	UE_LOG(LogArticyRuntime, Log, TEXT("Package %s loaded successfully."), *PackageName);
}

bool UArticyDatabase::UnloadPackage(const FString PackageName, const bool bQuickUnload)
{
	if(!LoadedPackages.Contains(PackageName))
	{
		UE_LOG(LogArticyRuntime, Log, TEXT("Package %s can't be unloaded due to not being loaded in the first place."), *PackageName);
		return false;
	}

	if (!ImportedPackages.Contains(PackageName) || ImportedPackages[PackageName] == nullptr)
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("Failed to find Package %s in imported packages!"), *PackageName);
		return false;
	}

	UArticyPackage* Package = ImportedPackages[PackageName];	

	for(auto ArticyObject : Package->GetAssets())
	{
		FArticyId ArticyId = ArticyObject->GetId();
		FName TechnicalName = ArticyObject->GetTechnicalName();

		bool bShouldUnload = false;
		if(bQuickUnload)
		{
			bShouldUnload = true;
		}
		else
		{
			/*
			 *	An exported object can exist multiple times in different packages
			 *  In the database, there can only be one object with the same Id, so if we are unloading slowly, check if the object is contained in another package first
			 *  Only unload if it is not contained in any other package
			*/
			bool bIsAssetContained = false;
			for(FString LoadedPackageName : LoadedPackages)
			{
				if (!LoadedPackageName.Equals(Package->Name))
				{
					bIsAssetContained = bIsAssetContained || ImportedPackages[LoadedPackageName]->GetAssetsDict().Contains(TechnicalName);
				}
			}

			if(!bIsAssetContained)
			{
				bShouldUnload = true;
			}
		}

		if(bShouldUnload)
		{
			LoadedObjectsById.FindAndRemoveChecked(ArticyId);
			LoadedObjectsByName.FindAndRemoveChecked(TechnicalName);
		}
	}

	LoadedPackages.Remove(Package->Name);
	UE_LOG(LogArticyRuntime, Log, TEXT("Package %s unloaded successfully."), *PackageName);

	return true;
}

void UArticyDatabase::UnloadAllPackages()
{
	LoadedPackages.Reset();
	LoadedObjectsById.Reset();
	LoadedObjectsByName.Reset();
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

		if(AssetData.Num() != 0)
		{
			if(AssetData.Num() > 1)
			{
				UE_LOG(LogArticyRuntime, Warning, TEXT("More than one ArticyDraftDatabase was found, this is not supported! The first one will be selected."));
			}

			Asset = Cast<UArticyDatabase>(AssetData[0].GetAsset());

			if(bLoadAllPackages && Asset.IsValid())
				Asset.Get()->LoadAllPackages();
		}
		else
		{
			UE_LOG(LogArticyRuntime, Warning, TEXT("No ArticyDraftDatabase was found."));
		}
	}

	return Asset.Get();
}

TWeakObjectPtr<UArticyDatabase> UArticyDatabase::GetMutableOriginal()
{
	static TWeakObjectPtr<UArticyDatabase> Asset = nullptr;

	if (!Asset.IsValid())
	{
		//create a clone of the database
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(StaticClass()->GetFName(), AssetData, true);

		if (AssetData.Num() != 0)
		{
			if (AssetData.Num() > 1)
			{
				UE_LOG(LogArticyRuntime, Warning, TEXT("More than one ArticyDraftDatabase was found, this is not supported! The first one will be selected."));
			}

			Asset = Cast<UArticyDatabase>(AssetData[0].GetAsset());
		}
		else
		{
			UE_LOG(LogArticyRuntime, Warning, TEXT("No ArticyDraftDatabase was found."));
		}
	}

	return Asset;
}

void UArticyDatabase::ChangePackageDefault(FName PackageName, bool bIsDefaultPackage)
{
	if (ImportedPackages.Contains(PackageName.ToString()))
	{
		UArticyPackage* Package = ImportedPackages[PackageName.ToString()];

		// additional check; maybe the map entry exists but has no value due to user error or bugs
		if(!Package)
		{
			UE_LOG(LogArticyRuntime, Error, TEXT("Could not change package default setting of package %s: package not found"), *PackageName.ToString());
			return;
		}

		// if the setting isn't going to change, return early
		if (Package->bIsDefaultPackage == bIsDefaultPackage)
		{
			return;
		}
		
		Package->bIsDefaultPackage = bIsDefaultPackage;
		Package->MarkPackageDirty();
	}
}

//---------------------------------------------------------------------------//

UArticyObject* UArticyDatabase::GetObject(FArticyId Id, int32 CloneId, TSubclassOf<class UArticyObject> CastTo) const
{
	return GetObjectInternal(Id, CloneId);
}

UArticyObject* UArticyDatabase::GetObjectUnshadowed(FArticyId Id, int32 CloneId) const
{
	return GetObjectInternal(Id, CloneId, true);
}

UArticyObject* UArticyDatabase::GetObjectInternal(FArticyId Id, int32 CloneId, bool bForceUnshadowed) const
{
	UArticyCloneableObject* const * info = LoadedObjectsById.Find(Id);
	return info && (*info) ? (*info)->Get(this, CloneId, bForceUnshadowed) : nullptr;
}

UArticyObject* UArticyDatabase::GetObjectByName(FName TechnicalName, int32 CloneId, TSubclassOf<class UArticyObject> CastTo) const
{
	auto arr = LoadedObjectsByName.Find(TechnicalName);
	if(!arr || arr->Objects.Num() <= 0)
		return nullptr;

	auto info = arr->Objects[0];
	return info? Cast<UArticyObject>(info->Get(this, CloneId)) : nullptr;
}

TArray<UArticyObject*> UArticyDatabase::GetObjects(FName TechnicalName, int32 CloneId, TSubclassOf<class UArticyObject> CastTo) const
{
	return GetObjects<UArticyObject>(TechnicalName, CloneId);
}

TArray<UArticyObject*> UArticyDatabase::GetObjectsOfClass(TSubclassOf<class UArticyObject> Type, int32 CloneId) const
{
	TArray<UArticyObject*> arr;
	TArray<UArticyCloneableObject*> Objects;
	LoadedObjectsById.GenerateValueArray(Objects);

	for (auto ClonableObject : Objects)
	{
		auto obj = ClonableObject->Get(this, CloneId, /*bForceUnshadowed = */ true);
		if (obj && (obj->GetCloneId() == CloneId) && obj->IsA(Type))
			arr.Add(Cast<UArticyObject>(obj));
	}

	return arr;
}

TArray<UArticyObject*> UArticyDatabase::GetAllObjects() const
{
	TArray<UArticyObject*> arr;
	TArray<UArticyCloneableObject*> Objects;
	LoadedObjectsById.GenerateValueArray(Objects);
	for (auto ClonableObject : Objects)
	{
		auto obj = ClonableObject->Get(this, 0, /*bForceUnshadowed = */ true);
			arr.Add(obj);
	}
	return arr;
}

//---------------------------------------------------------------------------//

UArticyObject* UArticyDatabase::CloneFrom(FArticyId Id, int32 NewCloneId, TSubclassOf<class UArticyObject> CastTo)
{
	auto info = LoadedObjectsById.Find(Id);
	return info? (*info)->Clone(this, NewCloneId, true) : nullptr;
}

UArticyObject* UArticyDatabase::CloneFromByName(FName TechnicalName, int32 NewCloneId, TSubclassOf<class UArticyObject> CastTo)
{
	auto arr = LoadedObjectsByName.Find(TechnicalName);
	if(!arr || arr->Objects.Num() <= 0)
		return nullptr;

	auto info = arr->Objects[0];
	return info? Cast<UArticyObject>(info->Clone(this, NewCloneId, true)) : nullptr;
}

//---------------------------------------------------------------------------//

UArticyObject* UArticyDatabase::GetOrClone(FArticyId Id, int32 NewCloneId)
{
	auto info = LoadedObjectsById.Find(Id);
	return info? (*info)->Clone(this, NewCloneId, false) : nullptr;
}

UArticyObject* UArticyDatabase::GetOrCloneByName(const FName& TechnicalName, int32 NewCloneId)
{
	auto arr = LoadedObjectsByName.Find(TechnicalName);
	if(!arr || arr->Objects.Num() <= 0)
		return nullptr;

	auto info = arr->Objects[0];
	return info? Cast<UArticyObject>(info->Clone(this, NewCloneId, false)) : nullptr;
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

UArticyDatabase::FAssetId UArticyDatabase::ResolveIDs(const FString& articyAssetFileName)
{
	FString fileName = FPaths::GetBaseFilename(articyAssetFileName);
	FAssetId assetId;
	FString technicalName;
	fileName.Split(TEXT("_"), &technicalName, &(assetId.s_uniqueID), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	assetId.technicalName = FName(*technicalName);
	auto uniqueIDStart = *(assetId.s_uniqueID) + 2;
	auto uniqueIDEnd = &(assetId.s_uniqueID[assetId.s_uniqueID.Len() - 1]);
	assetId.uniqueID = FCString::Strtoui64(uniqueIDStart, &uniqueIDEnd, 16);
	
	return assetId;
}
TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<UArticyDatabase>> UArticyDatabase::Clones;
TWeakObjectPtr<UArticyDatabase> UArticyDatabase::PersistentClone;