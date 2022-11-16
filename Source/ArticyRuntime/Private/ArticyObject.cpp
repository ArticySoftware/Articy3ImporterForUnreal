//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//


#include "ArticyObject.h"
#include "ArticyHelpers.h"
#include "ArticyDatabase.h"
#include "ArticyBaseTypes.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ArticyPackage.h"

#if WITH_EDITOR
TSet<TWeakObjectPtr<UArticyPackage>> UArticyObject::CachedPackages;
TMap<FArticyId, TWeakObjectPtr<UArticyObject>> UArticyObject::ArticyIdCache;
TMap<FName, TWeakObjectPtr<UArticyObject>> UArticyObject::ArticyNameCache;
#endif

void UArticyObject::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if(!Json.IsValid() || Json->Type != EJson::Object)
		return;

	auto obj = Json->AsObject();
	if(!ensure(obj.IsValid()))
		return;

	JSON_TRY_HEX_ID(obj, Parent);
	JSON_TRY_STRING(obj, TechnicalName);
}

//---------------------------------------------------------------------------//

FName UArticyObject::GetTechnicalName() const
{
	return *TechnicalName;
}

UArticyObject* UArticyObject::GetParent() const
{
	return UArticyDatabase::Get(this)->GetObject<UArticyObject>(Parent);
}

TArray<TWeakObjectPtr<UArticyObject>> UArticyObject::GetChildren() const
{	
	if (CachedChildren.Num() != Children.Num())
	{
		auto db = UArticyDatabase::Get(this);
		CachedChildren.Empty(Children.Num());

		for (auto childId : Children)
			if (auto child = db->GetObject<UArticyObject>(childId))
				CachedChildren.Add(child);
	}

	return CachedChildren;
}

FArticyId UArticyObject::GetParentID() const
{
	return Parent;
}

TArray<FArticyId> UArticyObject::GetChildrenIDs() const
{
	return Children;
}

#if WITH_EDITOR
TArray<FArticyId> UArticyObject::GetArticyObjectChildrenIDs() const
{
	TArray<FArticyId> OutIDs;

	for(auto ChildID : Children)
	{
		if(const UArticyObject* Object = UArticyObject::FindAsset(ChildID))
		{
			OutIDs.Add(Object->GetId());
		}
	}

	return OutIDs;
}

UArticyObject* UArticyObject::FindAsset(const FArticyId& Id)
{
	if(ArticyIdCache.Contains(Id) && ArticyIdCache[Id].IsValid())
	{
		return ArticyIdCache[Id].Get();
	}

	bool bRefreshPackages = false;
	
	if(CachedPackages.Num() >= 1)
	{
		for(auto& Package : CachedPackages)
		{
			bRefreshPackages = !Package.IsValid();

			if(bRefreshPackages)
			{
				break;
			}
		}
	}
	else if(CachedPackages.Num() == 0)
	{
		bRefreshPackages = true;
	}

	// refresh packages if needed 
	if(bRefreshPackages)
	{
		CachedPackages.Empty();
		
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
		TArray<FAssetData> AssetData;

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0
		AssetRegistryModule.Get().GetAssetsByClass( UArticyPackage::StaticClass()->GetClassPathName() , AssetData, true);
#else
		AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetFName(), AssetData, true);
#endif

		for (const auto ArticyPackage : AssetData)
		{
			const auto Package = Cast<UArticyPackage>(ArticyPackage.GetAsset());

			if (Package != nullptr)
			{
				CachedPackages.Add(Package);
			}
		}
	}

	for (TWeakObjectPtr<UArticyPackage> ArticyPackage : CachedPackages)
	{
		if (ArticyPackage.IsValid())
		{
			UArticyObject* ArticyObject = ArticyPackage->GetAssetById(Id);
			if (ArticyObject && ArticyObject->WasLoaded())
			{
				ArticyIdCache.Add(Id, ArticyObject);
				ArticyNameCache.Add(ArticyObject->GetTechnicalName(), ArticyObject);
				return ArticyObject;
			}
		}
	}

	// if the object wasn't found in any package but exists in the object map, remove it
	if(ArticyIdCache.Contains(Id))
	{
		ArticyIdCache.Remove(Id);
	}

	return nullptr;
}

UArticyObject* UArticyObject::FindAsset(const FString& TechnicalName)// MM_CHANGE
{
	const FName Name(*TechnicalName);
	if (ArticyNameCache.Contains(Name) && ArticyNameCache[Name].IsValid())
	{
		return ArticyNameCache[Name].Get();
	}
	
	bool bRefreshPackages = false;

	if (CachedPackages.Num() >= 1)
	{
		for (auto& Package : CachedPackages)
		{
			bRefreshPackages = !Package.IsValid();

			if (bRefreshPackages)
			{
				break;
			}
		}
	}
	else if (CachedPackages.Num() == 0)
	{
		bRefreshPackages = true;
	}

	// refresh packages if needed 
	if (bRefreshPackages)
	{
		CachedPackages.Empty();

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
		TArray<FAssetData> AssetData;

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0
		AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetClassPathName() , AssetData, true);
#else
		AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetFName(), AssetData, true);
#endif
		for (const auto ArticyPackage : AssetData)
		{
			const auto Package = Cast<UArticyPackage>(ArticyPackage.GetAsset());

			if (Package != nullptr)
			{
				CachedPackages.Add(Package);
			}
		}
	}
	
	for (TWeakObjectPtr<UArticyPackage> ArticyPackage : CachedPackages)
	{
		if (ArticyPackage.IsValid())
		{
			UArticyObject* ArticyObject = ArticyPackage->GetAssetByTechnicalName(Name);
			if (ArticyObject && ArticyObject->WasLoaded())
			{
				ArticyNameCache.Add(Name, ArticyObject);
				ArticyIdCache.Add(ArticyObject->GetId(), ArticyObject);
				return ArticyObject;
			}
		}
	}

	// if the object wasn't found in any package but exists in the object map, remove it
	if (ArticyNameCache.Contains(Name))
	{
		ArticyNameCache.Remove(Name);
	}

	return nullptr;
}
#endif
