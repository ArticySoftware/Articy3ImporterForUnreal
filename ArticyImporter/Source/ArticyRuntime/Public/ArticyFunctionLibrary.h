//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArticyObject.h"

#include "ArticyFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ARTICYRUNTIME_API UArticyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Converts an ArticyRef to an object. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "GetObject", DefaultToSelf = "WorldContext"), Category="ArticyRef")
	static UArticyObject* ArticyRef_GetObject(UPARAM(Ref) const FArticyRef& Ref, const UObject* WorldContext);
	/** Converts an ArticyRef to FArticyId. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "GetObjectId", DefaultToSelf = "WorldContext", BlueprintAutoCast), Category="ArticyRef")
	static FArticyId ArticyRef_GetObjectId(UPARAM(Ref) const FArticyRef& Ref);

	/** Converts an ArticyId to an object. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "GetObject", DefaultToSelf = "WorldContext"), Category="ArticyId")
	static UArticyObject* ArticyId_GetObject(UPARAM(Ref) const FArticyId& Id, const UObject* WorldContext);

	/** Creates an ArticyId from a hex string. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "ToArticyId"), Category="ArticyId")
	static FArticyId ArticyId_FromString(UPARAM(Ref) const FString& hex);

	/** Gets the HEX string from an ID. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "ToHexString"), Category="ArticyId")
	static FString ArticyId_ToString(UPARAM(Ref) const FArticyId& Id);

	/** Gets the last object in a branch. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "GetTarget"), Category="ArticyBranch")
	static TScriptInterface<class IArticyFlowObject> GetBranchTarget(UPARAM(ref) const struct FArticyBranch& Branch);
};
