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

	Types.Add(TEXT("ArticyString"), new ArticyPredefinedTypeInfo<UArticyString, UArticyString*>("UArticyString", "UArticyString*", TEXT("nullptr"), [](PROP_SETTER_PARAMS)
	{
		UArticyString* NewString = NewObject<UArticyString>();
		if (Json->Type == EJson::String)
		{
			NewString->Set(Json->AsString());
		}
		return NewString;
	}));
	Types.Add(TEXT("ArticyMultiLanguageString"), new ArticyPredefinedTypeInfo<UArticyMultiLanguageString, UArticyMultiLanguageString*>("UArticyMultiLanguageString", "UArticyMultiLanguageString*", TEXT("nullptr"), [](PROP_SETTER_PARAMS)
	{
		UArticyMultiLanguageString* NewString = NewObject<UArticyMultiLanguageString>();
		if (Json->Type == EJson::String)
		{
			NewString->Set(Json->AsString());
		}
		return NewString;
	}));

	Types.Add(TEXT("id"), PREDEFINE_TYPE(FArticyId));
	Types.Add(TEXT("string"), PREDEFINE_TYPE_EXT(FString, "TEXT(\"\")", [](PROP_SETTER_PARAMS) { return Json->Type == EJson::String ? Json->AsString() : FString{}; }));
	Types.Add(TEXT("ftext"), PREDEFINE_TYPE_EXT(FText, TEXT("FText::GetEmpty()"), [](PROP_SETTER_PARAMS)
		{
		if(Json->Type == EJson::String)
		{
			// Convert Unity rich text markup to Unreal (if the setting is enabled)
			FString Processed = GetDefault<UArticyPluginSettings>()->bConvertUnityToUnrealRichText ?
				ConvertUnityMarkupToUnreal(Json->AsString()) : 
				Json->AsString();

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

// Stores open tags in ConvertUnityMarkupToUnreal
struct TagInfo
{
	TagInfo(const FString& name, const FString& val) 
		: tagName(name), hasValue(val.Len() > 0), value(val), dummy(false) { 
		if (tagName == TEXT("align")) { dummy = true; }
	}

	// Tag name, like b, i, u, or color
	FString tagName;

	// Does this have a value? (like color="#FFFFFF")
	bool hasValue;

	// Value (if hasValue is true)
	FString value;

	// Dummy. Ignore this in the output
	bool dummy;
};

bool HasAnyTags(const TArray<TagInfo>& currentTags)
{
	for (const auto& tag : currentTags)
	{
		if (!tag.dummy) { return true; }
	}

	return false;
}

FString CreateOpenTag(const TArray<TagInfo>& currentTags)
{
	TArray<FString> tags;
	FString valueString = "";

	int numberOfTags = 0;
	for (const auto& tag : currentTags)
	{
		// Ignore dummy tags
		if (tag.dummy) continue;

		// If it's a value, append to the value string
		if (tag.hasValue) {
			valueString = valueString + FString::Printf(TEXT(" %s=\"%s\""), *tag.tagName, *tag.value);
		}
		else {
			// Otherwise, add the tag to the list
			tags.Add(tag.tagName);
		}
		numberOfTags++;
	}

	if (numberOfTags == 0) { return TEXT(""); }

	// Sort tag names
	tags.Sort([](const FString& a, const FString& b) { return (*a)[0] < (*b)[0]; });
	FString totalTags = "";
	for (const auto& tag : tags) { totalTags += tag; }

	// Handle no tags case
	if (totalTags.Len() == 0) { totalTags = "style"; }

	// Create tag
	return FString::Printf(TEXT("<%s%s>"), *totalTags, *valueString);
}

FString ConvertUnityMarkupToUnreal(const FString& Input)
{
	// Create a pattern to find closing tags
	static FRegexPattern Pattern(TEXT("<\\/.+?>|<(\\w+)(=\"?(.+?)\"?)?>"));

	// Create a matcher to search the input
	FRegexMatcher myMatcher(Pattern, Input);

	// Check to see if there's any matches at all
	bool anyMatches = myMatcher.FindNext();

	// If not, just return the input string
	if (!anyMatches) { return Input; }

	// Create a buffer to hold the output
	FString strings = "";
	/*
	// Future TODO: I wanted to use string builder for efficiency, but it's not available in older versions of Unreal
	//  or, at least, it gave me some build errors
	TCHAR* buffer = new TCHAR[Input.Len() * 2];
	FStringBuilderBase strings(buffer, Input.Len() * 2);
	*/

	// Run through matches
	TArray<TagInfo> currentTags;
	int last = 0;
	do
	{
		// Get bounds of match
		int start = myMatcher.GetMatchBeginning();
		int end = myMatcher.GetMatchEnding();

		// Add all text preceding the match to the output
		strings += (Input.Mid(last, start - last));

		// Check if we're dealing with a start tag or an end tag
		FString tagName = myMatcher.GetCaptureGroup(1);
		if (tagName.Len() > 0) {
			bool hasTagsToClose = HasAnyTags(currentTags);

			// Add to our list
			FString value = myMatcher.GetCaptureGroup(3);
			TagInfo info = TagInfo(tagName, value);
			currentTags.Add(info);

			// Don't bother if this is a dummy tag we're ignoring
			if (!info.dummy)
			{
				// If we have tags to close, close them
				if (hasTagsToClose) { strings += (TEXT("</>")); }

				// Open the tag
				strings += (CreateOpenTag(currentTags));
			}
		}
		else {
			// Remove our last tag
			auto popped = currentTags.Pop();

			// Only do the rest if the closed tag is not a dummy
			if (!popped.dummy)
			{
				// Write out the close
				strings += (TEXT("</>"));

				// If any tags are left, reopen
				if (currentTags.Num() > 0)
				{
					strings += (CreateOpenTag(currentTags));
				}
			}
		}

		last = end;

		
	} while (myMatcher.FindNext());

	// Add end of string
	if (last != Input.Len())
	{
		strings += (Input.Mid(last, Input.Len() - last));
	}

	// Create string
	FString result = strings;//.ToString();

	// Clean memory
	// TODO - see above about old unreal vers
	//delete buffer;
	//buffer = nullptr;

	// Return result
	return result;
}