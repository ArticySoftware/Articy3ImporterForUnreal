#include "GVTypes.h"

#include "ArticyEditorModule.h"

FString FArticyGVar::GetCPPTypeString() const
{
	switch (Type)
	{
	case EArticyType::ADT_Boolean:
		return TEXT("UArticyBool");

	case EArticyType::ADT_Integer:
		return TEXT("UArticyInt");

	case EArticyType::ADT_String:
		return TEXT("UArticyString");

	default:
		return TEXT("Cannot get CPP type string, unknown type!");
	}
}

FString FArticyGVar::GetCPPValueString() const
{
	FString value;
	switch (Type)
	{
	case EArticyType::ADT_Boolean:
		value = FString::Printf(TEXT("%s"), BoolValue ? TEXT("true") : TEXT("false"));
		break;

	case EArticyType::ADT_Integer:
		value = FString::Printf(TEXT("%d"), IntValue);
		break;

	case EArticyType::ADT_String:
		value = FString::Printf(TEXT("\"%s\""), *StringValue);
		break;

	default:
		value = TEXT("Cannot get CPP init string, unknown type!");
	}

	return value;
}

void FArticyGVar::ImportFromJson(const TSharedPtr<FJsonObject> JsonVar)
{
	if (!JsonVar.IsValid())
		return;

	JSON_TRY_STRING(JsonVar, Variable);
	JSON_TRY_STRING(JsonVar, Description);

	FString typeString;
	if (JsonVar->TryGetStringField(TEXT("Type"), typeString))
	{
		if (typeString == TEXT("Boolean"))
			Type = EArticyType::ADT_Boolean;
		else if (typeString == TEXT("Integer"))
			Type = EArticyType::ADT_Integer;
		else
		{
			if (typeString != TEXT("String"))
				UE_LOG(LogArticyEditor, Error, TEXT("Unknown GlobalVariable type '%s', falling back to String."),
				   *typeString);

			Type = EArticyType::ADT_String;
		}
	}

	switch (Type)
	{
	case EArticyType::ADT_Boolean: JsonVar->TryGetBoolField(TEXT("Value"), BoolValue);
		break;
	case EArticyType::ADT_Integer: JsonVar->TryGetNumberField(TEXT("Value"), IntValue);
		break;
	case EArticyType::ADT_String: JsonVar->TryGetStringField(TEXT("Value"), StringValue);
		break;
	default: break;
	}
}
