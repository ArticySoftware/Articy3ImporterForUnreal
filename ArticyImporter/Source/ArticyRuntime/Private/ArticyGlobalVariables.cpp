//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "Kismet/KismetStringLibrary.h"
#include "ArticyRuntime.h"
#include "ArticyGlobalVariables.h"
#include "ArticyPluginSettings.h"
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
	if(!Clone.IsValid())
	{
		bool keepBetweenWorlds = UArticyPluginSettings::Get()->bKeepGlobalVariablesBetweenWorlds;

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

		auto world = GEngine->GetWorldFromContextObjectChecked(WorldContext);
		ensureMsgf(world, TEXT("Getting world for GV cloning failed!"));

		if(keepBetweenWorlds)
		{
			Clone = DuplicateObject(asset, world->GetGameInstance());
#if !WITH_EDITOR
			Clone->AddToRoot();
#endif
		}
		else
		{
			Clone = DuplicateObject(asset, world);
		}

		ensureMsgf(Clone.IsValid(), TEXT("Cloning GV asset failed!"));
	}

	return Clone.Get();
}

void UArticyGlobalVariables::UnloadGlobalVariables()
{
	if (Clone.IsValid())
	{
		Clone->RemoveFromRoot();
		Clone->ConditionalBeginDestroy();
		Clone = NULL;
	}
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

const bool& UArticyGlobalVariables::GetBoolVariable(const FName Namespace, const FName Variable, bool& bSucceeded)
{
	return GetVariableValue<UArticyBool, bool>(Namespace, Variable, bSucceeded);
}

const int32& UArticyGlobalVariables::GetIntVariable(const FName Namespace, const FName Variable, bool& bSucceeded)
{
	return GetVariableValue<UArticyInt, int32>(Namespace, Variable, bSucceeded);
}

const FString& UArticyGlobalVariables::GetStringVariable(const FName Namespace, const FName Variable, bool& bSucceeded)
{
	return GetVariableValue<UArticyString, FString>(Namespace, Variable, bSucceeded);
}

void UArticyGlobalVariables::SetBoolVariable(const FName Namespace, const FName Variable, const bool Value)
{
	SetVariableValue<UArticyBool>(Namespace, Variable, Value);
}

void UArticyGlobalVariables::SetIntVariable(const FName Namespace, const FName Variable, const int32 Value)
{
	SetVariableValue<UArticyInt>(Namespace, Variable, Value);
}

void UArticyGlobalVariables::SetStringVariable(const FName Namespace, const FName Variable, const FString Value)
{
	SetVariableValue<UArticyString>(Namespace, Variable, Value);
}

TWeakObjectPtr<UArticyGlobalVariables> UArticyGlobalVariables::Clone;