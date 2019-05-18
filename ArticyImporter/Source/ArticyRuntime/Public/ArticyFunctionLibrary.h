//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArticyObject.h"
#include "ArticyFlowPlayer.h"
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
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Object", DefaultToSelf = "WorldContext", DeterminesOutputType = "CastTo"), Category="ArticyRef")
	static UArticyObject* ArticyRef_GetObject(UPARAM(Ref) const FArticyRef& Ref, TSubclassOf<class UArticyObject> CastTo, const UObject* WorldContext);
	/** Sets the referenced object of an ArticyRef */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Object", DefaultToSelf = "WorldContext"), Category = "ArticyRef")
	static void ArticyRef_SetObject(UPARAM(Ref) FArticyRef& Ref, UPARAM(Ref) UArticyPrimitive* Object);
	/** Converts an ArticyRef to FArticyId. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Object Id", DefaultToSelf = "WorldContext", BlueprintAutoCast), Category="ArticyRef")
	static FArticyId ArticyRef_GetObjectId(UPARAM(Ref) const FArticyRef& Ref);
	/** Sets the referenced object of an ArticyRef */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Object Id", DefaultToSelf = "WorldContext", BlueprintAutoCast), Category = "ArticyRef")
	static void ArticyRef_SetObjectId(UPARAM(Ref) FArticyRef& Ref, UPARAM(Ref) FArticyId Id);

	/** Converts an ArticyId to an object. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Object", DefaultToSelf = "WorldContext", DeterminesOutputType = "CastTo"), Category="ArticyId")
	static UArticyObject* ArticyId_GetObject(UPARAM(Ref) const FArticyId& Id, TSubclassOf<class UArticyObject> CastTo, const UObject* WorldContext);

	/** Creates an ArticyId from a hex string. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "To Articy Id"), Category="ArticyId")
	static FArticyId ArticyId_FromString(UPARAM(Ref) const FString& hex);

	/** Gets the HEX string from an ID. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "To Hex String"), Category="ArticyId")
	static FString ArticyId_ToString(UPARAM(Ref) const FArticyId& Id);

	/** Returns true if ArticyId A is equal to ArticyId B */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (ArticyId)", CompactNodeTitle = "==", Keywords = "== equal"), Category = "ArticyId")
	static bool ArticyId_Equal(const FArticyId& A, const FArticyId& B);
	/** Returns true if ArticyId A is not equal to ArticyId B */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Not Equal (ArticyId)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = "ArticyId")
	static bool ArticyId_NotEqual(const FArticyId& A, const FArticyId& B);
	/** Returns true if ArticyId is valid: object non-null and underlying id non-zero */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Valid"), Category = "ArticyId")
	static bool ArticyId_IsValid(const FArticyId& Id);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Make ArticyGvName from full name"), Category = "ArticyId")
	static FArticyGvName ArticyGvName_MakeFromFullName(const FName& FullName);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Make ArticyGvName from namespace & variable"), Category = "ArticyId")
	static FArticyGvName ArticyGvName_MakeFromVariableAndNamespace(const FName& Variable, const FName& Namespace);


	/** Gets the last object in a branch. */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Target"), Category="ArticyBranch")
	static TScriptInterface<class IArticyFlowObject> GetBranchTarget(UPARAM(ref) const struct FArticyBranch& Branch);
};
