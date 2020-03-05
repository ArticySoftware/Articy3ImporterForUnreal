//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyFunctionLibrary.h"
#include "ArticyObject.h"
#include "ArticyFlowPlayer.h"

UArticyObject* UArticyFunctionLibrary::ArticyRef_GetObject(const FArticyRef& Ref, TSubclassOf<class UArticyObject> CastTo, const UObject* WorldContext)
{
	return Ref.GetObject(WorldContext);
}

void UArticyFunctionLibrary::ArticyRef_SetObject(FArticyRef& Ref, UArticyObject* Object)
{
	Ref.SetReference(Object);
}

FArticyId UArticyFunctionLibrary::ArticyRef_GetObjectId(const FArticyRef& Ref)
{
	return Ref.GetId();
}

void UArticyFunctionLibrary::ArticyRef_SetObjectId(FArticyRef& Ref, FArticyId Id)
{
	Ref.SetId(Id);
}

UArticyObject* UArticyFunctionLibrary::ArticyId_GetObject(const FArticyId& Id, TSubclassOf<class UArticyObject> CastTo, const UObject* WorldContext)
{
	return Cast<UArticyObject>(Id.GetObject(WorldContext));
}

FArticyId UArticyFunctionLibrary::ArticyId_FromString(const FString& hex)
{
	return FArticyId{ hex };
}

FString UArticyFunctionLibrary::ArticyId_ToString(const FArticyId& Id)
{
	return ArticyHelpers::Uint64ToHex(Id);
}

bool UArticyFunctionLibrary::ArticyId_Equal(const FArticyId& A, const FArticyId& B)
{
	return A == B;
}

bool UArticyFunctionLibrary::ArticyId_NotEqual(const FArticyId& A, const FArticyId& B)
{
	return A != B;
}

bool UArticyFunctionLibrary::ArticyId_IsValid(const FArticyId & Id)
{
	return Id && Id.Low != 0 && Id.High != 0;
}

FArticyGvName UArticyFunctionLibrary::ArticyGvName_MakeFromFullName(const FName& FullName)
{
	return FArticyGvName(FullName);
}

FArticyGvName UArticyFunctionLibrary::ArticyGvName_MakeFromVariableAndNamespace(const FName& Variable, const FName& Namespace)
{
	return FArticyGvName(Variable, Namespace);
}

TScriptInterface<IArticyFlowObject> UArticyFunctionLibrary::GetBranchTarget(const FArticyBranch& Branch)
{
	return Branch.GetTarget();
}
 