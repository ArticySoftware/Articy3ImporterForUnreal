#include "Manifest.h"

#include "ImportFactories/Data/ArticyImportData.h"

void FADIManifest::ImportFromJson(const TSharedPtr<FJsonObject> JsonRoot)
{
	if (!JsonRoot.IsValid())
		return;

	// @Alewinn : Why defining own macros "JSON_TRY_stuff..."
	// if we need to continue to use UE primitives in some cases ?
	// This just over-complexify things, as we have to maintain 2 syntax
	// strategies instead of the primitive UE one... ><'
	GlobalVariablesHash.ImportFromJson(JsonRoot
										->GetObjectField("GlobalVariables"));
	ObjectDefinitionsTypeHash.ImportFromJson(JsonRoot
										->GetObjectField("ObjectDefinitions")
										->GetObjectField("Types"));
	ObjectDefinitionsTextsHash.ImportFromJson(JsonRoot
											->GetObjectField("ObjectDefinitions")
											->GetObjectField("Texts"));
	// @todo : packages...
	// PackagesFilesHashes
	

	ScriptMethodsHash.ImportFromJson(JsonRoot->GetObjectField("ScriptMethods"));
	HierarchyHash.ImportFromJson(JsonRoot->GetObjectField("Hierarchy"));
	
	const TArray<TSharedPtr<FJsonValue>>* varsJson;
	// @Alewinn : Dunno why "JSON_TRY_ARRAY is not that much used in the 
	// already existing code...Will use UE primitives as much as possible
	// by default, to try to lower code complexity.
	if (!JsonRoot->TryGetArrayField("Languages", varsJson)) 
		return;
	
	for (const TSharedPtr<FJsonValue> varJson : *varsJson)
	{
		const TSharedPtr<FJsonObject> obj = varJson->AsObject();
		if (!obj.IsValid())
			continue;
		
		FADILocalizedLanguage lang;
		lang.ImportFromJson(obj);
		Languages.Add(lang);
	}
}
