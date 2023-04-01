#include "AIDUserMethodsProxy.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

FAIDUserMethods AIDUserMethodsProxy::fromJson(const TArray<TSharedPtr<FJsonValue>>* Json)
{
	_userMethods.ScriptMethods.Reset(Json ? Json->Num() : 0);

	if (!Json)
		return {};

	TSet<FString> OverloadedMethods;

	for (const auto smJson : *Json)
	{
		const auto obj = smJson->AsObject();
		if (!obj.IsValid())
			continue;

		FAIDScriptMethod sm;
		GetScriptMethod(sm,obj, OverloadedMethods);
		_userMethods.ScriptMethods.Add(sm);
	}

	// mark overloaded methods => Not poco friendly. move elsewhere (if necessary)
	for (auto& scriptMethod : _userMethods.ScriptMethods)
		scriptMethod.bIsOverloadedFunction = OverloadedMethods.Contains(scriptMethod.Name);

	return _userMethods;
}

void AIDUserMethodsProxy::GetScriptMethod(FAIDScriptMethod& scriptMethod,TSharedPtr<FJsonObject> Json, TSet<FString>& OverloadedMethods)
{
	Json->TryGetStringField(TEXT("Name"), scriptMethod.Name );
	Json->TryGetStringField(TEXT("ReturnType"), scriptMethod.ReturnType );

	scriptMethod.BlueprintName = scriptMethod.Name + TEXT("_");
	scriptMethod.ParameterList.Empty();
	scriptMethod.OriginalParameterTypes.Empty();

	const TArray<TSharedPtr<FJsonValue>>* items;

	if (Json->TryGetArrayField(TEXT("Parameters"), items))
	{
		for (const auto item : *items)
		{
			const TSharedPtr<FJsonObject>* obj;
			if (!ensure(item->TryGetObject(obj))) continue;

			//import parameter name and type
			FString Param, Type;
			(*obj)->TryGetStringField(TEXT("Param"), Param );
			(*obj)->TryGetStringField(TEXT("Type"), Type );

			// append param types to blueprint names
			FString formattedType = Type;
			formattedType[0] = FText::FromString(Type).ToUpper().ToString()[0];
			scriptMethod.BlueprintName += formattedType;

			scriptMethod.OriginalParameterTypes.Add(Type);

			//append to parameter list
			FAIDScriptMethodParameter param;
			param.Name = Param;
			param.Type = Type,
			scriptMethod.ParameterList.Emplace(param);
			scriptMethod.ArgumentList.Add(Param);
		}
	}

	if (scriptMethod.BlueprintName.EndsWith("_"))
		scriptMethod.BlueprintName.RemoveAt(scriptMethod.BlueprintName.Len() - 1);

	// determine if this is an overloaded blueprint function
	static TMap<FString, FString> UsedBlueprintMethodsNames;
	if (UsedBlueprintMethodsNames.Contains(scriptMethod.Name))
	{
		if (UsedBlueprintMethodsNames[scriptMethod.Name] != scriptMethod.BlueprintName)
			OverloadedMethods.Add(scriptMethod.Name);
	}
	else
	{
		UsedBlueprintMethodsNames.Add(scriptMethod.Name, scriptMethod.BlueprintName);
	}
}
