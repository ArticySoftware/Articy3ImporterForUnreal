#include "UserMethods.h"

#include "ScriptMethod.h"
#include "Dom/JsonValue.h"

void FAIDUserMethods::ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json)
{
	ScriptMethods.Reset(Json ? Json->Num() : 0);

	if (!Json)
		return;

	TSet<FString> OverloadedMethods;

	for (const auto smJson : *Json)
	{
		const auto obj = smJson->AsObject();
		if (!obj.IsValid())
			continue;

		FAIDScriptMethod sm;
		sm.ImportFromJson(obj, OverloadedMethods);
		ScriptMethods.Add(sm);
	}

	// mark overloaded methods
	for (auto& scriptMethod : ScriptMethods)
		scriptMethod.bIsOverloadedFunction = OverloadedMethods.Contains(scriptMethod.Name);
}
