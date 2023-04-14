#include "ScriptMethod.h"

#include "ArticyHelpers.h"
#include "MethodParameter.h"

const FString& FAIDScriptMethod::GetCPPReturnType() const
{
	//TODO change this once the ReturnType is changed from C#-style ('System.Void' ecc.) to something more generic!
	if (ReturnType == "string")
	{
		const static auto String = FString{"const FString"};
		return String;
	}
	if (ReturnType == "object")
	{
		// object is pretty much all encompassing. We only support them as UArticyPrimitives right now, which means GetObj(...) and self works.
		const static auto ArticyPrimitive = FString{"UArticyPrimitive*"};
		return ArticyPrimitive;
	}

	return ReturnType;
}

const FString& FAIDScriptMethod::GetCPPDefaultReturn() const
{
	//TODO change this once the ReturnType is changed from C#-style ('System.Void' ecc.) to something more generic!
	if (ReturnType == "bool")
	{
		const static auto True = FString{"true"};
		return True;
	}
	if (ReturnType == "int" || ReturnType == "float")
	{
		const static auto Zero = FString{"0"};
		return Zero;
	}
	if (ReturnType == "string")
	{
		const static auto EmptyString = FString{"\"\""};
		return EmptyString;
	}
	if (ReturnType == "ArticyObject")
	{
		const static auto ArticyObject = FString{"nullptr"};
		return ArticyObject;
	}

	const static auto Nothing = FString{""};
	return Nothing;
}

const FString FAIDScriptMethod::GetCPPParameters() const
{
	FString Parameters = "";

	for (const auto& Parameter : ParameterList)
	{
		FString Type = Parameter.Type;

		if (Type.Equals("string"))
		{
			Type = TEXT("const FString&");
		}
		else if (Type.Equals("object"))
		{
			Type = TEXT("UArticyPrimitive*");
		}

		Parameters += Type + TEXT(" ") + Parameter.Name + TEXT(", ");
	}

	return Parameters.LeftChop(2);
}

const FString FAIDScriptMethod::GetArguments() const
{
	FString Parameters = "";

	for (const auto& Argument : ArgumentList)
	{
		Parameters += Argument + TEXT(", ");
	}

	return Parameters.LeftChop(2);
}

const FString FAIDScriptMethod::GetOriginalParametersForDisplayName() const
{
	FString DisplayNameSuffix = "";

	for (const auto& OriginalParameterType : OriginalParameterTypes)
	{
		DisplayNameSuffix += OriginalParameterType + TEXT(", ");
	}

	return DisplayNameSuffix.LeftChop(2);
}

void FAIDScriptMethod::ImportFromJson(TSharedPtr<FJsonObject> Json, TSet<FString>& OverloadedMethods)
{
	JSON_TRY_STRING(Json, Name);
	JSON_TRY_STRING(Json, ReturnType);

	BlueprintName = Name + TEXT("_");
	ParameterList.Empty();
	OriginalParameterTypes.Empty();

	const TArray<TSharedPtr<FJsonValue>>* items;

	if (Json->TryGetArrayField(TEXT("Parameters"), items))
	{
		for (const auto item : *items)
		{
			const TSharedPtr<FJsonObject>* obj;
			if (!ensure(item->TryGetObject(obj))) continue;

			//import parameter name and type
			FString Param, Type;
			JSON_TRY_STRING((*obj), Param);
			JSON_TRY_STRING((*obj), Type);

			// append param types to blueprint names
			FString formattedType = Type;
			formattedType[0] = FText::FromString(Type).ToUpper().ToString()[0];
			BlueprintName += formattedType;

			OriginalParameterTypes.Add(Type);

			//append to parameter list
			ParameterList.Emplace(Type, Param);
			ArgumentList.Add(Param);
		}
	}

	if (BlueprintName.EndsWith("_"))
		BlueprintName.RemoveAt(BlueprintName.Len() - 1);

	// determine if this is an overloaded blueprint function
	static TMap<FString, FString> UsedBlueprintMethodsNames;
	if (UsedBlueprintMethodsNames.Contains(Name))
	{
		if (UsedBlueprintMethodsNames[Name] != BlueprintName)
			OverloadedMethods.Add(Name);
	}
	else
	{
		UsedBlueprintMethodsNames.Add(Name, BlueprintName);
	}
}
