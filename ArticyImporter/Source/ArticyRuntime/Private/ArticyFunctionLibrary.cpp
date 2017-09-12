//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "ArticyFunctionLibrary.h"
#include "ArticyObject.h"
#include "ArticyFlowPlayer.h"

UArticyObject* UArticyFunctionLibrary::ArticyRef_GetObject(const FArticyRef& Ref, const UObject* WorldContext)
{
	return Ref.GetObject(WorldContext);
}

FArticyId UArticyFunctionLibrary::ArticyRef_GetObjectId(const FArticyRef& Ref)
{
	return Ref.GetId();
}

UArticyObject* UArticyFunctionLibrary::ArticyId_GetObject(const FArticyId& Id, const UObject* WorldContext)
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

TScriptInterface<IArticyFlowObject> UArticyFunctionLibrary::GetBranchTarget(const FArticyBranch& Branch)
{
	return Branch.GetTarget();
}
 