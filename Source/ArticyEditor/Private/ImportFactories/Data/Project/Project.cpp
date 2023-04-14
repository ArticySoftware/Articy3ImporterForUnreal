#include "Project.h"

#include "ArticyHelpers.h"

void FArticyProjectDef::ImportFromJson(const TSharedPtr<FJsonObject> Json)
{
	if (!Json.IsValid())
		return;

	JSON_TRY_STRING(Json, Name);
	JSON_TRY_STRING(Json, DetailName);
	JSON_TRY_STRING(Json, Guid);
	JSON_TRY_STRING(Json, TechnicalName);
}
