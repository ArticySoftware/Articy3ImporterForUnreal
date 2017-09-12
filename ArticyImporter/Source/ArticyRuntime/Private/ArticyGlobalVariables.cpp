//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "ArticyRuntime.h"
#include "ArticyGlobalVariables.h"
#include "ArticyFlowPlayer.h"

uint32 UArticyVariable::GetStoreShadowLevel() const
{
	return Store->GetShadowLevel();
}

void UArticyBaseVariableSet::BroadcastOnVariableChanged(UArticyVariable* Variable)
{
	OnVariableChanged.Broadcast(Variable);
}

//---------------------------------------------------------------------------//

UArticyGlobalVariables* UArticyGlobalVariables::GetDefault(const UObject* WorldContext)
{
	static TWeakObjectPtr<UArticyGlobalVariables> Clone = nullptr;

	if(!Clone.IsValid())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		TArray<FAssetData> AssetData;
		AssetRegistryModule.Get().GetAssetsByClass(UArticyGlobalVariables::StaticClass()->GetFName(), AssetData, true);

		UArticyGlobalVariables* asset = nullptr;
		if(ensureMsgf(AssetData.Num() != 0, TEXT("ArticyGlobalVariables asset not found!")))
		{
			if(AssetData.Num() > 1)
			UE_LOG(LogTemp, Warning, TEXT("More than one ArticyGlobalVariables asset was found, this is not supported! The first one will be selected."));

			asset = Cast<UArticyGlobalVariables>(AssetData[0].GetAsset());
		}

		if(!asset)
			return nullptr;

		UE_LOG(LogTemp, Warning, TEXT("Cloning GVs."));

		auto world = GEngine->GetWorldFromContextObject(WorldContext);
		ensureMsgf(world, TEXT("Getting world for GV cloning failed!"));

		Clone = DuplicateObject(asset, world); //TODO need to add to root object?
		ensureMsgf(Clone.IsValid(), TEXT("Cloning GV asset failed!"));
	}

	return Clone.Get();
}

UArticyBaseVariableSet* UArticyGlobalVariables::GetNamespace(const FName Namespace)
{
	auto set = GetProp<UArticyBaseVariableSet*>(Namespace);
	if(!set)
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("GV Namespace %s not found!"), *Namespace.ToString());
		return nullptr;
	}

	return set;
}

const bool& UArticyGlobalVariables::GetBoolVariable(FName Namespace, const FName Variable, bool& bSucceeded)
{
	auto set = GetNamespace(Namespace);
	if(set)
	{
		auto ptr = set->GetPropPtr<bool>(Variable);
		if(ptr)
		{
			bSucceeded = true;
			return *ptr;
		}
	}

	bSucceeded = false;
	UE_LOG(LogArticyRuntime, Warning, TEXT("Could not find GV %s.%s!"), *Namespace.ToString(), *Variable.ToString());
	static bool Empty = false;
	return Empty;
}

const int32& UArticyGlobalVariables::GetIntVariable(FName Namespace, const FName Variable, bool& bSucceeded)
{
	auto set = GetNamespace(Namespace);
	if(set)
	{
		auto ptr = set->GetPropPtr<int32>(Variable);
		if(ptr)
		{
			bSucceeded = true;
			return *ptr;
		}
	}

	bSucceeded = false;
	UE_LOG(LogArticyRuntime, Warning, TEXT("Could not find GV %s.%s!"), *Namespace.ToString(), *Variable.ToString());
	static int32 Empty = -1;
	return Empty;
}

const FString& UArticyGlobalVariables::GetStringVariable(const FName Namespace, const FName Variable, bool& bSucceeded)
{
	auto set = GetNamespace(Namespace);
	if(set)
	{
		auto ptr = set->GetPropPtr<FString>(Variable);
		if(ptr)
		{
			bSucceeded = true;
			return *ptr;
		}
	}

	bSucceeded = false;
	UE_LOG(LogArticyRuntime, Warning, TEXT("Could not find GV %s.%s!"), *Namespace.ToString(), *Variable.ToString());
	static FString Empty = "";
	return Empty;
}

void UArticyGlobalVariables::SetBoolVariable(const FName Namespace, const FName Variable, const bool Value)
{
	auto set = GetNamespace(Namespace);
	if(set)
		set->SetProp<bool>(Variable, Value);
}

void UArticyGlobalVariables::SetIntVariable(const FName Namespace, const FName Variable, const int32 Value)
{
	auto set = GetNamespace(Namespace);
	if(set)
		set->SetProp<int32>(Variable, Value);
}

void UArticyGlobalVariables::SetStringVariable(const FName Namespace, const FName Variable, const FString Value)
{
	auto set = GetNamespace(Namespace);
	if(set)
		set->SetProp<FString>(Variable, Value);
}
