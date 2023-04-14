#include "LocalizedLanguage.h"

#include "ArticyHelpers.h"


void FADILocalizedLanguage::ImportFromJson(const TSharedPtr<FJsonObject> Json)
{
	if (!Json.IsValid())
		return;

	JSON_TRY_STRING(Json, CultureName);
	JSON_TRY_INT(Json,ArticyLanguageId);

	// What is the type of this field ? Nullable string ? ... 
	JSON_TRY_STRING(Json, LanguageName);
}
