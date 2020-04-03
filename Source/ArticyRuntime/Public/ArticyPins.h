//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyObject.h"
#include "ArticyHelpers.h"
#include "ArticyBaseTypes.h"
#include "Interfaces/ArticyFlowObject.h"
#include "Interfaces/ArticyConditionProvider.h"
#include "Interfaces/ArticyInstructionProvider.h"

#include "ArticyPins.generated.h"

class UArticyOutgoingConnection;
/**
 * A flow fragment input- or output pin.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyFlowPin : public UArticyPrimitive, public IArticyFlowObject
{
	GENERATED_BODY()

public:
	/** The script fragment. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Text = "";

	/** The Id of the object owning this pin. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FArticyId Owner;

	/** All outgoing connections. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UArticyOutgoingConnection*> Connections;

	void InitFromJson(TSharedPtr<FJsonValue> Json) override;

	UFUNCTION(BlueprintCallable)
	UArticyObject* GetOwner();

	//---------------------------------------------------------------------------//

	EArticyPausableType GetType() override { return EArticyPausableType::Pin; }

	//stub implementation
	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override { ensure(false); }
};

/**
 * An input pin.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyInputPin : public UArticyFlowPin, public IArticyConditionProvider
{
	GENERATED_BODY()

public:

	EArticyPausableType GetType() override { return UArticyFlowPin::GetType(); }

	//---------------------------------------------------------------------------//

	bool Evaluate(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) override;

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;
};

/**
 * An ouput pin.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyOutputPin : public UArticyFlowPin, public IArticyInstructionProvider
{
	GENERATED_BODY()

public:

	EArticyPausableType GetType() override { return UArticyFlowPin::GetType(); }

	//---------------------------------------------------------------------------//

	void Execute(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) override;

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;
};
