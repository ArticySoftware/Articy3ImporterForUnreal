//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "ArticyObjectWith_Base.h"
#include "UObject/TextProperty.h"
#include "Engine/Engine.h"
#include "Internationalization/StringTable.h"
#include "ArticyObjectWithText.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, meta=(CannotImplementInterfaceInBlueprint))
class UArticyObjectWithText : public UArticyObjectWith_Base { GENERATED_BODY() };

/**
 * All objects that have a property called 'Text' implement this interface.
 */
class IArticyObjectWithText : public IArticyObjectWith_Base
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithText")
	virtual FText GetText()
	{
		static const auto PropName = FName("Text");
		FText& Key = GetProperty<FText>(PropName);
		const FText MissingEntry = FText::FromString("<MISSING STRING TABLE ENTRY>");

		/* const FString KeyAsString = Key.ToString();
		const int32 DelimiterIndex = KeyAsString.Find(TEXT(":"));
		if (DelimiterIndex == INDEX_NONE)
		{
			// Look up entry in default string table
			const FText SourceString = FText::FromStringTable(
				TEXT("ArticyStrings"),
				KeyAsString,
				EStringTableLoadingPolicy::FindOrFullyLoad);
			if (!SourceString.IsEmpty() && !SourceString.EqualTo(MissingEntry))
			{
				return SourceString;
			}

			// By default, return the key
			return Key;
		}

		// Look up entry in specified string table
		const FString TableName = KeyAsString.Left(DelimiterIndex);
		const FString KeyName = KeyAsString.RightChop(DelimiterIndex + 1); 
		const FText SourceString = FText::FromStringTable(
			FName(TableName),
			KeyName,
			EStringTableLoadingPolicy::FindOrFullyLoad);
		const FString Decoded = SourceString.ToString();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Decoded);
		if (!SourceString.IsEmpty() && !SourceString.EqualTo(MissingEntry))
		{
			return SourceString;
		} */
		
		const FText SourceString = FText::FromStringTable(
			TEXT("ArticyStrings"),
			Key.ToString(),
			EStringTableLoadingPolicy::FindOrFullyLoad);
		if (!SourceString.IsEmpty() && !SourceString.EqualTo(MissingEntry))
		{
			return SourceString;
		}

		// By default, return the key
		return Key;
	}

	virtual FText GetText() const
	{
		return const_cast<IArticyObjectWithText*>(this)->GetText();
	}
	
	//---------------------------------------------------------------------------//

	UFUNCTION(BlueprintCallable, Category="ArticyObjectWithText")
	virtual FText& SetText(UPARAM(ref) const FText& Text)
	{
		return GetText() = Text;
	}
};
