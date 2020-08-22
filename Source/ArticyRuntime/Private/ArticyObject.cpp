//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//


#include "ArticyObject.h"
#include "ArticyHelpers.h"
#include "ArticyDatabase.h"
#include "ArticyBaseTypes.h"
#include "AssetRegistryModule.h"

#include <ArticyPackage.h>

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
	//UArticyDatabase::LoadAllObjects();

	//get the reference object by Id
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	TArray<FAssetData> AssetData;

	AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetFName(), AssetData, true);

	for (const auto ArticyPackage : AssetData)
	{
		const auto Package = Cast<UArticyPackage>(ArticyPackage.GetAsset());

		if (Package != nullptr)
		{
			for (const auto asset : Package->GetAssets())
			{
				const auto obj = Cast<UArticyObject>(asset);
				if (obj && obj->WasLoaded() && obj->GetId() == Id)
					return obj;
			}
		}
	}

	return nullptr;
}

UArticyObject* UArticyObject::FindAsset(const FString& TechnicalName)// MM_CHANGE
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	TArray<FAssetData> AssetData;

	AssetRegistryModule.Get().GetAssetsByClass(UArticyPackage::StaticClass()->GetFName(), AssetData, true);

	for (const auto ArticyPackage : AssetData)
	{
		const auto Package = Cast<UArticyPackage>(ArticyPackage.GetAsset());

		if (Package != nullptr)
		{
			const auto assetsDict = Package->GetAssetsDict();

			if (assetsDict.Contains(*TechnicalName))
				return assetsDict[*TechnicalName].Get();
		}
	}

	return nullptr;
}