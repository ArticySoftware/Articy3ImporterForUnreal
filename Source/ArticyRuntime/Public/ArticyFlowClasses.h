//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyPins.h"
#include "Interfaces/ArticyNode.h"

#include "ArticyFlowClasses.generated.h"

/**
 * 
 */
UCLASS()
class ARTICYRUNTIME_API UArticyFlowFragment : public UArticyNode
{
	GENERATED_BODY()

public:

	EArticyPausableType GetType() override { return EArticyPausableType::FlowFragment; }
};

/**
 * 
 */
UCLASS()
class ARTICYRUNTIME_API UArticyDialogue : public UArticyNode
{
	GENERATED_BODY()
	
public:

	EArticyPausableType GetType() override { return EArticyPausableType::Dialog; }
};

/**
 * 
 */
UCLASS()
class ARTICYRUNTIME_API UArticyDialogueFragment : public UArticyNode
{
	GENERATED_BODY()
	
public:

	EArticyPausableType GetType() override { return EArticyPausableType::DialogFragment; }
};

/**
 * 
 */
UCLASS()
class ARTICYRUNTIME_API UArticyHub : public UArticyNode
{
	GENERATED_BODY()
	
public:

	EArticyPausableType GetType() override { return EArticyPausableType::Hub; }
};

/**
 * 
 */
UCLASS()
class ARTICYRUNTIME_API UArticyJump : public UArticyNode
{
	GENERATED_BODY()
	
public:

	EArticyPausableType GetType() override { return EArticyPausableType::Jump; }

	UArticyPrimitive* GetTarget() const;
	FArticyId GetTargetID() const { return Target; }
	UArticyFlowPin* GetTargetPin() const;
	FArticyId GetTargetPinID() const { return TargetPin; }

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="TargetPin"))
	FArticyId TargetPin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName="Target"))
	FArticyId Target;

private:

	UPROPERTY(VisibleAnywhere, Transient)
	mutable UArticyFlowPin* TargetPinObj;
	UPROPERTY(VisibleAnywhere, Transient)
	mutable UArticyPrimitive* TargetObj;
};
