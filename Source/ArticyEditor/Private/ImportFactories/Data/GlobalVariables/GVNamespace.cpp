#include "GVNamespace.h"

#include "ArticyHelpers.h"
#include "CodeGeneration/CodeGenerator.h"

void FArticyGVNamespace::ImportFromJson(const TSharedPtr<FJsonObject> JsonNamespace, const UArticyImportData* Data)
{
	if (!JsonNamespace.IsValid())
		return;

	JSON_TRY_STRING(JsonNamespace, Namespace);
	CppTypename = CodeGenerator::GetGVNamespaceClassname(Data, Namespace);
	JSON_TRY_STRING(JsonNamespace, Description);

	const TArray<TSharedPtr<FJsonValue>>* varsJson;
	if (!JsonNamespace->TryGetArrayField(TEXT("Variables"), varsJson))
		return;
	for (const auto varJson : *varsJson)
	{
		const auto obj = varJson->AsObject();
		if (!obj.IsValid())
			continue;

		FArticyGVar var;
		var.ImportFromJson(obj);
		Variables.Add(var);
	}
}
