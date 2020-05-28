//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "ArticyBaseObject.h"
#include "Interfaces/ArticyNode.h"
#include "Interfaces/ArticyConditionProvider.h"
#include "Interfaces/ArticyInstructionProvider.h"
#include "ArticyScriptFragment.generated.h"

/**
 * Base class for the script fragments (ArticyScriptCondition or ArticyScriptInstruction)
 */
UCLASS(BlueprintType, abstract)
class ARTICYRUNTIME_API UArticyScriptFragment : public UArticyPrimitive
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Articy")
	FString Expression = "";

	//returns a cached hash of the expression
	int GetExpressionHash() const;

	template<typename Type, typename PropType>
		friend struct ArticyObjectTypeInfo;

	void InitFromJson(TSharedPtr<FJsonValue> Json) override;

private:

	mutable int CachedExpressionHash;
};

/** -------------------------------------------------------------------------------- */

/**
 * A script condition.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyScriptCondition : public UArticyScriptFragment, public IArticyConditionProvider
{
	GENERATED_BODY()

public:

	bool Evaluate(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) override;
};

/**
 * A condition node, which has a script condition.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyCondition : public UArticyNode, public IArticyConditionProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Condition")
	UArticyScriptCondition* GetCondition() const;

	EArticyPausableType GetType() override { return EArticyPausableType::Condition; }

	bool Evaluate(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) override;

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;

private:

	UPROPERTY(transient)
	mutable UArticyScriptCondition* Condition = nullptr;
};

/** -------------------------------------------------------------------------------- */

/**
 * A script instruction.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyScriptInstruction : public UArticyScriptFragment, public IArticyInstructionProvider
{
	GENERATED_BODY()

public:

	void Execute(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) override;
};

/**
 * A instruction node, which has a script instruction.
 */
UCLASS(BlueprintType)
class ARTICYRUNTIME_API UArticyInstruction : public UArticyNode, public IArticyInstructionProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Instruction")
	UArticyScriptInstruction* GetInstruction() const;

	EArticyPausableType GetType() override { return EArticyPausableType::Instruction; }

	void Execute(class UArticyGlobalVariables* GV = nullptr, class UObject* MethodProvider = nullptr) override;

	void Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth) override;
	
private:

	UPROPERTY(transient)
	mutable UArticyScriptInstruction* Instruction = nullptr;
};
