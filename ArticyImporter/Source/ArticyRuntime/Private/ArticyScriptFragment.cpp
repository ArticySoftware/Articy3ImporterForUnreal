//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#include "ArticyRuntimePrivatePCH.h"

#include "ArticyScriptFragment.h"
#include "ArticyExpressoScripts.h"

FName UArticyScriptFragment::GetExpression() const
{
	if(CachedExpression.IsNone() && !Expression.IsEmpty())
		CachedExpression = *Expression;

	return CachedExpression;
}

void UArticyScriptFragment::InitFromJson(TSharedPtr<FJsonValue> Json)
{
	Super::InitFromJson(Json);

	if (!Json.IsValid() || !ensure(Json->Type == EJson::String))
		return;

	//import the fragment expression
	auto exp = Json->AsString();
	if(!exp.IsEmpty())
		Expression = *exp;
}

//---------------------------------------------------------------------------//

bool UArticyScriptCondition::Evaluate(class UArticyGlobalVariables* GV, class UObject* MethodProvider)
{
	auto db = UArticyDatabase::Get(this);
	return db->GetExpressoInstance()->Evaluate(GetExpression(), GV ? GV : db->GetGVs(), MethodProvider);
}

bool UArticyCondition::Evaluate(UArticyGlobalVariables* GV, UObject* MethodProvider)
{
	return !GetCondition() || GetCondition()->Evaluate(GV, MethodProvider);
}

void UArticyCondition::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	auto pins = GetOutputPins();
	if(!pins || !ensure(pins->Num() == 2))
	{
		//conditions MUST have 2 output pins!
		//continue on output pins, will also handle the case where pins are not valid
		Super::Explore(Player, OutBranches, Depth);
		return;
	}

	if(Evaluate(Player->GetGVs(), Player->GetMethodsProvider()))
		OutBranches.Append( Player->Explore((*pins)[0], false, Depth+1) ); //TRUE
	else
		OutBranches.Append( Player->Explore((*pins)[1], false, Depth+1) ); //FALSE
}

//---------------------------------------------------------------------------//

void UArticyScriptInstruction::Execute(class UArticyGlobalVariables* GV, class UObject* MethodProvider)
{
	auto db = UArticyDatabase::Get(this);
	db->GetExpressoInstance()->Execute(GetExpression(), GV ? GV : db->GetGVs(), MethodProvider);
}

UArticyScriptCondition* UArticyCondition::GetCondition() const
{
	if(!Condition)
	{
		FName Expression = TEXT("Expression");
		Condition = GetProp<UArticyScriptCondition*>(Expression);
	}

	return Condition;
}

UArticyScriptInstruction* UArticyInstruction::GetInstruction() const
{
	if(!Instruction)
	{
		FName Expression = TEXT("Expression");
		Instruction = GetProp<UArticyScriptInstruction*>(Expression);
	}

	return Instruction;
}

void UArticyInstruction::Execute(UArticyGlobalVariables* GV, UObject* MethodProvider)
{
	if(GetInstruction())
		GetInstruction()->Execute(GV, MethodProvider);
}

void UArticyInstruction::Explore(UArticyFlowPlayer* Player, TArray<FArticyBranch>& OutBranches, const uint32& Depth)
{
	Execute(Player->GetGVs(), Player->GetMethodsProvider());

	//continue on output pins
	Super::Explore(Player, OutBranches, Depth);
}
