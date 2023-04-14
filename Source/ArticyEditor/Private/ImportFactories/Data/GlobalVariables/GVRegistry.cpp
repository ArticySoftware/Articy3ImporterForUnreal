#include "GVRegistry.h"

#include "GVNamespace.h"
#include "Dom/JsonValue.h"

void FArticyGVInfo::ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json, const UArticyImportData* Data)
{
	Namespaces.Reset(Json ? Json->Num() : 0);

	if (!Json)
		return;

	for (const auto nsJson : *Json)
	{
		const auto obj = nsJson->AsObject();
		if (!obj.IsValid())
			continue;

		FArticyGVNamespace ns;
		ns.ImportFromJson(obj, Data);
		Namespaces.Add(ns);
	}
}
