#include "ArticyProjectDefProxy.h"

#include "Dom/JsonObject.h"

FArticyProjectDef FArticyProjectDefProxy::fromJson(const TSharedPtr<FJsonObject> JsonProjectDef)
{
	if (!JsonProjectDef.IsValid())
		return {};

	JsonProjectDef->TryGetStringField(TEXT("Name"), _project.Name);
	JsonProjectDef->TryGetStringField(TEXT("DetailName"), _project.DetailName);
	JsonProjectDef->TryGetStringField(TEXT("Guid"), _project.Guid);
	JsonProjectDef->TryGetStringField(TEXT("TechnicalName"), _project.TechnicalName);

	return _project;
}
