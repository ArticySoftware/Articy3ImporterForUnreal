//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyTexts.h"
#include "ArticyHelpers.h"
#include "PredefinedTypes.h"

void FArticyTextDef::ImportFromJson(const TSharedPtr<FJsonValue>& Json)
{
	if (!Json.IsValid())
		return;

	const TSharedPtr<FJsonObject> JsonObject = Json->AsObject();
	if (JsonObject->Values.IsEmpty())
	{
		Text = ConvertUnityMarkupToUnreal(Json->AsString());
		return;
	}
	
	JSON_TRY_STRING(JsonObject, Text);
	Text = ConvertUnityMarkupToUnreal(Text);
	JSON_TRY_STRING(JsonObject, VOAsset);
}

void FArticyTexts::ImportFromJson(const TSharedPtr<FJsonObject>& Json)
{
	if (!Json.IsValid())
		return;

	for (auto JsonValue = Json->Values.CreateConstIterator(); JsonValue; ++JsonValue)
	{
		const FString Name = (*JsonValue).Key;
	
		if (Name.Equals(TEXT("Context")))
		{
			JSON_TRY_STRING(Json, Context); 
		}
		else
		{
			FArticyTextDef Def;
			const TSharedPtr<FJsonValue> Value = (*JsonValue).Value;
			Def.ImportFromJson(Value);
			Content.Add(Name, Def);
		}
	}
}
