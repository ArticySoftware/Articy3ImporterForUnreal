#include "ADISettingsProxy.h"

FADISettings ADISettingsProxy::fromJson(const TSharedPtr<FJsonObject> JsonSettings)
{
	if (!JsonSettings.IsValid())
		return {};

	JsonSettings->TryGetStringField(TEXT("set_TextFormatter"), _settings.set_TextFormatter);
	JsonSettings->TryGetBoolField(TEXT("set_UseScriptSupport"), _settings.set_UseScriptSupport);
	JsonSettings->TryGetStringField(TEXT("ExportVersion"), _settings.ExportVersion);
	JsonSettings->TryGetStringField(TEXT("ObjectDefinitionsHash"), _settings.ObjectDefinitionsHash);
	JsonSettings->TryGetStringField(TEXT("ScriptFragmentsHash"), _settings.ScriptFragmentsHash);

	return _settings;
}
