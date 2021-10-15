//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "PredefinedTypes.h"
#include "ArticyImportData.h"
#include "ArticyObject.h"
#include "ArticyBuiltinTypes.h"
#include "ArticyScriptFragment.h"

#define STRINGIFY(x) #x

// Converts Unity rich text markup to Unreal rich text markup.
// Amounts to just replacing all closing tags with </> as Unreal
// does not include the tag name in the closing tag.
//
// Ex. "My text has <b>bold</b> words." to "My text has <b>bold</> words."
FString ConvertUnityMarkupToUnreal(const FString& Input);

//---------------------------------------------------------------------------//

//make sure the static Map is filled
FArticyPredefTypes FArticyPredefTypes::StaticInstance = FArticyPredefTypes{};

FArticyPredefTypes::FArticyPredefTypes()
{
	//generic type for enums
	Enum = MakeShareable(PREDEFINE_TYPE_EXT(uint8, "", [](PROP_SETTER_PARAMS)
			{
				uint32 num;
				Json->TryGetNumber(num);
				return static_cast<uint8>(num);
			}));

	Types.Reset();

	//Add all predefined types
	//The key is the original type. It is NOT case-sensitive (FName)!
	//Some are exposed as values, some as pointers
	//Some can be initialized from json, others can't!
	
	Types.Add(TEXT("ArticyPrimitive"), new ArticyPredefinedTypeInfo<UArticyPrimitive, UArticyPrimitive*>("UArticyPrimitive", "UArticyPrimitive*", TEXT("nullptr"), nullptr /* NOTE: NO INITIALIZATION FROM JSON! */));
	Types.Add(TEXT("ArticyObject"), new ArticyPredefinedTypeInfo<UArticyObject, UArticyObject*>("UArticyObject", "UArticyObject*", TEXT("nullptr"), nullptr /* NOTE: NO INITIALIZATION FROM JSON! */));

	Types.Add(TEXT("id"), PREDEFINE_TYPE(FArticyId));
	Types.Add(TEXT("string"), PREDEFINE_TYPE_EXT(FString, "TEXT(\"\")", [](PROP_SETTER_PARAMS) { return Json->Type == EJson::String ? Json->AsString() : FString{}; }));
	Types.Add(TEXT("ftext"), PREDEFINE_TYPE_EXT(FText, TEXT("FText::GetEmpty()"), [](PROP_SETTER_PARAMS)
		{
		if(Json->Type == EJson::String)
		{
			// Convert Unity rich text markup to Unreal
			FString Processed = ConvertUnityMarkupToUnreal(Json->AsString());

			//return a new FText, where the Path is the key and the Property value is the defaut-language text
			return FText::ChangeKey(TEXT("ARTICY"), Path, FText::FromString(Processed));
		}
		return FText::GetEmpty();
	}));
	Types.Add(TEXT("rect"), PREDEFINE_TYPE(FArticyRect));
	Types.Add(TEXT("color"), PREDEFINE_TYPE_EXT(FLinearColor, "FLinearColor::Black", [](PROP_SETTER_PARAMS) { return ArticyHelpers::ParseColorFromJson(Json); }));
	Types.Add(TEXT("point"), PREDEFINE_TYPE_EXT(FVector2D, "FVector2D::ZeroVector", [](PROP_SETTER_PARAMS) { return ArticyHelpers::ParseFVector2DFromJson(Json); }));
	Types.Add(TEXT("size"), PREDEFINE_TYPE(FArticySize));
	Types.Add(TEXT("float"), PREDEFINE_TYPE_EXT(float, "0.f", [](PROP_SETTER_PARAMS) { return Json->IsNull() ? 0.f : static_cast<float>(Json->AsNumber()); }));

	auto int32Info = PREDEFINE_TYPE_EXT(int32, "0", [](PROP_SETTER_PARAMS) {
												int32 num;
												Json->TryGetNumber(num);
												return num;
											});
	//uint is imported as int32 too, so we can expose it to blueprints
	Types.Add(TEXT("uint"), int32Info);
	Types.Add(TEXT("int"), int32Info);

	auto boolInfo = PREDEFINE_TYPE_EXT(bool, "false", [](PROP_SETTER_PARAMS) { return !Json->IsNull() && Json->AsBool(); });
	//bool and boolean are the same
	Types.Add(TEXT("bool"), boolInfo);
	Types.Add(TEXT("boolean"), boolInfo);

	Types.Add(TEXT("DateTime"), PREDEFINE_TYPE_EXT(FDateTime, "", [](PROP_SETTER_PARAMS)
	{
		FDateTime dt;
		if(Json->Type == EJson::String)
		{
			auto str = Json->AsString();
			int32 lastDot;
			if(str.FindLastChar('.', lastDot))
			{
				//unreal only allows for up to 3 digits for the fractional second -.-
				auto pos = lastDot + 3;
				str.RemoveAt(pos, str.Len()-pos);
			}

			ensure(FDateTime::ParseIso8601(*str, dt));
		}
		return dt;
	}));

	//generic arrays - the ItemType is filled in by FArticyPropertyDef
	//NOTE we cannot resolve the ItemType here, so we add a placeholder
	Types.Add(TEXT("array"), new ArticyPredefinedTypeInfo<nullptr_t>(TEXT("TArray<?>"), TEXT("TArray<?>"), "", nullptr));

	//========================================//

	//preview image
	/*
	deprecated, we have generic enum solution
	Types.Add(TEXT("PreviewImageViewBoxModes"),PREDEFINE_TYPE_EXT(EArticyPreviewImageViewBoxModes, [](PROP_SETTER_PARAMS)
												{
													uint32 num;
													Json->TryGetNumber(num);
													return static_cast<EArticyPreviewImageViewBoxModes>(num);
												}));*/

	Types.Add(TEXT("PreviewImage"), PREDEFINE_ARTICYOBJECT_TYPE(UArticyPreviewImage));
	
	//========================================//

	//connections
	Types.Add(TEXT("IncomingConnection"), PREDEFINE_ARTICYOBJECT_TYPE(UArticyIncomingConnection));
	Types.Add(TEXT("OutgoingConnection"), PREDEFINE_ARTICYOBJECT_TYPE(UArticyOutgoingConnection));

	//========================================//

	//pins
	Types.Add(TEXT("InputPin"), PREDEFINE_ARTICYOBJECT_TYPE(UArticyInputPin));
	Types.Add(TEXT("OutputPin"), PREDEFINE_ARTICYOBJECT_TYPE(UArticyOutputPin));

	//========================================//
	
	//script fragments
	Types.Add(TEXT("Script_Condition"), new ArticyObjectTypeInfo<UArticyScriptCondition, UArticyScriptCondition*>("UArticyScriptCondition", "UArticyScriptCondition""*"));
	Types.Add(TEXT("Script_Instruction"), new ArticyObjectTypeInfo<UArticyScriptInstruction, UArticyScriptInstruction*>("UArticyScriptInstruction", "UArticyScriptInstruction""*"));

	//========================================//

	//locations
	/*
	deprecated, we have generic enum solution
	Types.Add(TEXT("LocationAnchorSize"),PREDEFINE_TYPE(EArticyLocationAnchorSize, [](PROP_SETTER_PARAMS)
											{
												uint32 num;
												Json->TryGetNumber(num);
												return static_cast<EArticyLocationAnchorSize>(num);
											}));*/

	Types.Add(TEXT("LocationAnchor"), PREDEFINE_TYPE(FArticyLocationAnchor));

	Types.Add(TEXT("Transformation"), PREDEFINE_ARTICYOBJECT_TYPE(UArticyTransformation));
}

bool FArticyPredefTypes::IsPredefinedType(const FName& OriginalType)
{
	return StaticInstance.Types.Contains(OriginalType);
}

FString ConvertUnityMarkupToUnreal(const FString& Input)
{
	// Create a pattern to find closing tags
	static FRegexPattern Pattern(TEXT("<\\/.+?>"));

	// Create a matcher to search the input
	FRegexMatcher myMatcher(Pattern, Input);

	// Check to see if there's any matches at all
	bool anyMatches = myMatcher.FindNext();

	// If not, just return the input string
	if (!anyMatches) { return Input; }

	// Create a buffer to hold the output
	TCHAR* buffer = new TCHAR[Input.Len() + 1];
	FStringBuilderBase strings(buffer, Input.Len() + 1);

	// Run through matches
	int last = 0;
	do
	{
		// Get bounds of match
		int start = myMatcher.GetMatchBeginning();
		int end = myMatcher.GetMatchEnding();

		// Add all text preceeding the match to the output
		strings.Append(Input.Mid(last, start - last));

		// Replace the closing tag with just </>
		strings.Append(TEXT("</>"));
		last = end;
	} while (myMatcher.FindNext());

	// Add end of string
	if (last != Input.Len())
	{
		strings.Append(Input.Mid(last, Input.Len() - last));
	}

	return strings.ToString();
}