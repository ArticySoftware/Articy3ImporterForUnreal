#include "Settings.h"
#include "ArticyHelpers.h"

void FADISettings::ImportFromJson(TSharedPtr<FJsonObject> Json)
{
	if (!Json.IsValid())
		return;

	JSON_TRY_BOOL(Json, set_Localization);
	JSON_TRY_STRING(Json, set_TextFormatter);
	JSON_TRY_BOOL(Json, set_UseScriptSupport);
	JSON_TRY_STRING(Json, ExportVersion);

	const auto oldObjectDefsHash = ObjectDefinitionsHash;
	const auto oldScriptFragmentHash = ScriptFragmentsHash;
	JSON_TRY_STRING(Json, ObjectDefinitionsHash);
	JSON_TRY_STRING(Json, ScriptFragmentsHash);
	bObjectDefsOrGVsChanged = oldObjectDefsHash != ObjectDefinitionsHash;
	bScriptFragmentsChanged = oldScriptFragmentHash != ScriptFragmentsHash;
}
