//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//


#include "ArticyGlobalVariables.h"
#include "Kismet/KismetStringLibrary.h"
#include "ArticyRuntimeModule.h"
#include "ArticyPluginSettings.h"
#include "ArticyFlowPlayer.h"


FArticyGvName::FArticyGvName(const FName FullVariableName)
{
	SetByFullName(FullVariableName);
}

FArticyGvName::FArticyGvName(const FName VariableNamespace, const FName VariableName)
{
	SetByNamespaceAndVariable(VariableNamespace, VariableName);
}

void FArticyGvName::SetByFullName(const FName FullVariableName)
{
	FString variableString;
	FString namespaceString;
	if (FullVariableName.ToString().Split(TEXT("."), &namespaceString, &variableString))
	{
		FullName = FullVariableName;
		Namespace = FName(*namespaceString);
		Variable = FName(*variableString);
	}
}

void FArticyGvName::SetByNamespaceAndVariable(const FName VariableNamespace, const FName VariableName)
{
	if (!VariableNamespace.IsNone() && !VariableName.IsNone())
	{
		Namespace = VariableNamespace;
		Variable = VariableName;
		FullName = FName(*FString::Printf(TEXT("%s.%s"), *Namespace.ToString(), *Variable.ToString()));
	}
}

const FName& FArticyGvName::GetNamespace()
{
	if(!Namespace.IsNone())
		return Namespace;

	SetByFullName(FullName);
	return Namespace;
}

const FName& FArticyGvName::GetVariable()
{
	if(!Variable.IsNone())
		return Variable;

	SetByFullName(FullName);
	return Variable;
}

const FName& FArticyGvName::GetFullName()
{
	if(!FullName.IsNone())
		return FullName;

	SetByNamespaceAndVariable(Namespace, Variable);
	return FullName;
}

//---------------------------------------------------------------------------//

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
			Clone = DuplicateObject(asset, world->GetGameInstance(), TEXT("Persistent Runtime GV"));
#if !WITH_EDITOR
			Clone->AddToRoot();
#endif
		}
		else
		{
			Clone = DuplicateObject(asset, world, *FString::Printf(TEXT("%s GV"), *world->GetName()));
		}

		ensureMsgf(Clone.IsValid(), TEXT("Cloning GV asset failed!"));
	}

	return Clone.Get();
}

UArticyGlobalVariables* UArticyGlobalVariables::GetMutableOriginal()
{
	static TWeakObjectPtr<UArticyGlobalVariables> Asset = nullptr;

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
				UE_LOG(LogArticyRuntime, Warning, TEXT("More than one ArticyGV was found, this is not supported! The first one will be selected."));
			}

			Asset = Cast<UArticyGlobalVariables>(AssetData[0].GetAsset());
		}
		else
		{
			UE_LOG(LogArticyRuntime, Warning, TEXT("No ArticyDraftGV was found."));
		}
	}

	return Asset.Get();
}

UArticyGlobalVariables* UArticyGlobalVariables::GetRuntimeClone(const UObject* WorldContext, UArticyAlternativeGlobalVariables* GVs)
{
	// Special case: We're passed a nullptr. Use the default shared set.
	if (GVs == nullptr) { return GetDefault(WorldContext);  }

	// Get unique name of GV set
	const FString Name = GVs->GetFullName();
	const FName Key = FName(*Name);

	// Check if we already have a clone
	const auto Existing = OtherClones.Find(Key);
	if (Existing && Existing->IsValid())
	{
		// If so, return it
		return Existing->Get();
	}

	UE_LOG(LogTemp, Warning, TEXT("Cloning Override GVs %s"), *GVs->GetFullName());

	// Get world context
	auto world = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	ensureMsgf(world, TEXT("Getting world for GV cloning failed!"));

	// Get global variable asset to clone
	UArticyGlobalVariables* asset = UArticyGlobalVariables::GetMutableOriginal();

	// Check if we're keeping global variable objects between worlds
	bool keepBetweenWorlds = UArticyPluginSettings::Get()->bKeepGlobalVariablesBetweenWorlds;

	// If so, duplicate and add to root
	UArticyGlobalVariables* NewClone = nullptr;
	if (keepBetweenWorlds)
	{
		FString NewName = TEXT("Persistent Runtime GV Clone of ") + Name;
		NewClone = DuplicateObject(asset, world->GetGameInstance(), *NewName);
#if !WITH_EDITOR
		NewClone->AddToRoot();
#endif
	}
	else
	{
		// Otherwise, add it to the active world
		NewClone = DuplicateObject(asset, world, *FString::Printf(TEXT("%s %s GV"), *world->GetName(), *Name));
	}

	// Store and return
	OtherClones.FindOrAdd(Key) = NewClone;
	return NewClone;
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
		if(bLogVariableAccess)
		{
			UE_LOG(LogArticyRuntime, Error, TEXT("GV Namespace %s not found!"), *Namespace.ToString());
		}
		return nullptr;
	}

	return set;
}


void UArticyGlobalVariables::PrintGlobalVariable(FArticyGvName GvName)
{
	bool bTmpLogVariableAccess = bLogVariableAccess;
	bool bPrintSuccessful = false;
	// turn off variable access logging for this function so that the result only prints once
	bLogVariableAccess = false;
	
	auto set = GetNamespace(GvName.GetNamespace());
	if(set) 
	{
		UArticyVariable** basePtr = set->GetPropPtr<UArticyVariable*>(GvName.GetVariable());

		if (Cast<UArticyBool>(*basePtr))
		{
			bool boolSucceeded = false;
			auto boolValue = GetBoolVariable(GvName, boolSucceeded);
			UE_LOG(LogArticyRuntime, Display, TEXT("%s::%s = %s"), *GvName.GetNamespace().ToString(), *GvName.GetVariable().ToString(), boolValue ? *FString("True") : *FString("False"));
			bPrintSuccessful = true;
		}
		else if (Cast<UArticyInt>(*basePtr))
		{
			bool intSucceeded = false;
			auto intValue = GetIntVariable(GvName, intSucceeded);
			UE_LOG(LogArticyRuntime, Display, TEXT("%s::%s = %d"), *GvName.GetNamespace().ToString(), *GvName.GetVariable().ToString(), intValue);
			bPrintSuccessful = true;
		}
		else if (Cast<UArticyString>(*basePtr))
		{
			bool stringSucceeded = false;
			auto stringValue = GetStringVariable(GvName, stringSucceeded);
			UE_LOG(LogArticyRuntime, Display, TEXT("%s::%s = %s"), *GvName.GetNamespace().ToString(), *GvName.GetVariable().ToString(), *stringValue);
			bPrintSuccessful = true;
		}
	}

	if(!bPrintSuccessful)
	{
		UE_LOG(LogArticyRuntime, Error, TEXT("Unable to find variable: %s::%s"), *GvName.GetNamespace().ToString(), *GvName.GetVariable().ToString());
	}

	// restore prior logging mode
	bLogVariableAccess = bTmpLogVariableAccess;
}

const bool& UArticyGlobalVariables::GetBoolVariable(FArticyGvName GvName, bool& bSucceeded)
{
	return GetVariableValue<UArticyBool, bool>(GvName.GetNamespace(), GvName.GetVariable(), bSucceeded);
}

const int32& UArticyGlobalVariables::GetIntVariable(FArticyGvName GvName, bool& bSucceeded)
{
	return GetVariableValue<UArticyInt, int32>(GvName.GetNamespace(), GvName.GetVariable(), bSucceeded);
}

const FString& UArticyGlobalVariables::GetStringVariable(FArticyGvName GvName, bool& bSucceeded)
{
	return GetVariableValue<UArticyString, FString>(GvName.GetNamespace(), GvName.GetVariable(), bSucceeded);
}

void UArticyGlobalVariables::SetBoolVariable(FArticyGvName GvName, const bool Value)
{
	SetVariableValue<UArticyBool>(GvName.GetNamespace(), GvName.GetVariable(), Value);
}

void UArticyGlobalVariables::SetIntVariable(FArticyGvName GvName, const int32 Value)
{
	SetVariableValue<UArticyInt>(GvName.GetNamespace(), GvName.GetVariable(), Value);
}

void UArticyGlobalVariables::SetStringVariable(FArticyGvName GvName, const FString Value)
{
	SetVariableValue<UArticyString>(GvName.GetNamespace(), GvName.GetVariable(), Value);
}

void UArticyGlobalVariables::EnableDebugLogging()
{
	bLogVariableAccess = true;
}

void UArticyGlobalVariables::DisableDebugLogging()
{
	bLogVariableAccess = false;
}

TWeakObjectPtr<UArticyGlobalVariables> UArticyGlobalVariables::Clone;
TMap<FName, TWeakObjectPtr< UArticyGlobalVariables>> UArticyGlobalVariables::OtherClones;

