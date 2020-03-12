//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyImportData.h"
#include "EditorFramework/AssetImportData.h"
#include "CodeGeneration/CodeGenerator.h"
#include "ArticyJSONFactory.h"
#include "ArticyPluginSettings.h"
#include "Internationalization/Regex.h"
#include "ArticyEditorModule.h"

#define LOCTEXT_NAMESPACE "ArticyImportData"

void FADISettings::ImportFromJson(TSharedPtr<FJsonObject> Json)
{
	if(!Json.IsValid())
		return;

	JSON_TRY_BOOL(Json, set_Localization);
	JSON_TRY_STRING(Json, set_TextFormatter);
	JSON_TRY_BOOL(Json, set_UseScriptSupport);
	JSON_TRY_STRING(Json, ExportVersion);

	const auto oldObjectDefsHash = ObjectDefinitionsHash;
	const auto oldScriptFragmentHash = ScriptFragmentsHash;
	JSON_TRY_STRING(Json, ObjectDefinitionsHash);
	JSON_TRY_STRING(Json, ScriptFragmentsHash);
	bObjectDefsOrGVsChanged = oldObjectDefsHash != ObjectDefinitionsHash;
	bScriptFragmentsChanged = oldScriptFragmentHash != ScriptFragmentsHash;
	
}

void FArticyProjectDef::ImportFromJson(const TSharedPtr<FJsonObject> Json)
{
	if(!Json.IsValid())
		return;

	JSON_TRY_STRING(Json, Name);
	JSON_TRY_STRING(Json, DetailName);
	JSON_TRY_STRING(Json, Guid);
	JSON_TRY_STRING(Json, TechnicalName);
}

FString FArticyGVar::GetCPPTypeString() const
{
	switch(Type)
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
	switch(Type)
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
	if(!JsonVar.IsValid())
		return;

	JSON_TRY_STRING(JsonVar, Variable);
	JSON_TRY_STRING(JsonVar, Description);

	FString typeString;
	if(JsonVar->TryGetStringField(TEXT("Type"), typeString))
	{
		if(typeString == TEXT("Boolean"))
			Type = EArticyType::ADT_Boolean;
		else if(typeString == TEXT("Integer"))
			Type = EArticyType::ADT_Integer;
		else
		{
			if(typeString != TEXT("String"))
			UE_LOG(LogArticyEditor, Error, TEXT("Unknown GlobalVariable type '%s', falling back to String."), *typeString);

			Type = EArticyType::ADT_String;
		}
	}

	switch(Type)
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

void FArticyGVNamespace::ImportFromJson(const TSharedPtr<FJsonObject> JsonNamespace, const UArticyImportData* Data)
{
	if(!JsonNamespace.IsValid())
		return;

	JSON_TRY_STRING(JsonNamespace, Namespace);
	CppTypename = CodeGenerator::GetGVNamespaceClassname(Data, Namespace);
	JSON_TRY_STRING(JsonNamespace, Description);

	const TArray<TSharedPtr<FJsonValue>>* varsJson;
	if(!JsonNamespace->TryGetArrayField(TEXT("Variables"), varsJson))
		return;
	for(const auto varJson : *varsJson)
	{
		const auto obj = varJson->AsObject();
		if(!obj.IsValid())
			continue;

		FArticyGVar var;
		var.ImportFromJson(obj);
		Variables.Add(var);
	}
}

void FArticyGVInfo::ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json, const UArticyImportData* Data)
{
	Namespaces.Reset(Json ? Json->Num() : 0);

	if(!Json)
		return;

	for(const auto nsJson : *Json)
	{
		const auto obj = nsJson->AsObject();
		if(!obj.IsValid())
			continue;

		FArticyGVNamespace ns;
		ns.ImportFromJson(obj, Data);
		Namespaces.Add(ns);
	}
}

//---------------------------------------------------------------------------//

const FString& FAIDScriptMethod::GetCPPReturnType() const
{
	//TODO change this once the ReturnType is changed from C#-style ('System.Void' ecc.) to something more generic!
	if(ReturnType == "string")
	{
		const static auto String = FString{ "FString" };
		return String;
	}
	if(ReturnType == "ArticyObject")
	{
		const static auto ArticyObject = FString{ "UArticyObject*" };
		return ArticyObject;
	}
	
	return ReturnType;
}

const FString& FAIDScriptMethod::GetCPPDefaultReturn() const
{
	//TODO change this once the ReturnType is changed from C#-style ('System.Void' ecc.) to something more generic!
	if(ReturnType == "bool")
	{
		const static auto True = FString{ "true" };
		return True;
	}
	if(ReturnType == "int" || ReturnType == "float")
	{
		const static auto Zero = FString{ "0" };
		return Zero;
	}
	if(ReturnType == "string")
	{
		const static auto EmptyString = FString{ "\"\"" };
		return EmptyString;
	}
	if (ReturnType == "ArticyObject")
	{
		const static auto ArticyObject = FString{ "nullptr" };
		return ArticyObject;
	}
	
	const static auto Nothing = FString{ "" };
	return Nothing;
}

void FAIDScriptMethod::ImportFromJson(TSharedPtr<FJsonObject> Json, TSet<FString> &OverloadedMethods)
{
	JSON_TRY_STRING(Json, Name);
	JSON_TRY_STRING(Json, ReturnType);

	BlueprintName = Name + TEXT("_");
	ParameterList = TEXT("");
	OrigininalParameterTypes = TEXT("");
	const TArray<TSharedPtr<FJsonValue>>* items;

	if(Json->TryGetArrayField(TEXT("Parameters"), items))
	{
		for(const auto item : *items)
		{
			const TSharedPtr<FJsonObject>* obj;
			if(!ensure(item->TryGetObject(obj))) continue;

			//import parameter name and type
			FString Param, Type;
			JSON_TRY_STRING((*obj), Param);
			JSON_TRY_STRING((*obj), Type);
			
			// append param types to blueprint names
			FString formattedType = Type;
			formattedType[0] = FText::FromString(Type).ToUpper().ToString()[0];
			BlueprintName += formattedType;

			OrigininalParameterTypes += ", " + Type;

			//string -> const FString& (because UE4 wants a const reference for strings..)
			if(Type.Equals(TEXT("string")))
				Type = TEXT("const FString&");
			else if(Type.Equals(TEXT("ArticyObject")))
				Type = TEXT("UArticyObject*");

			//append to parameter list
			ParameterList += TEXT(", ") + Type + TEXT(" ") + Param;
			ArgumentList += TEXT(", ") + Param;
		}

		//remove the leading ", "
		if(ParameterList.Len() >= 2)
			ParameterList.RemoveAt(0, 2);
		if(ParameterList.Len() >= 2)
			ArgumentList.RemoveAt(0, 2);
		if(OrigininalParameterTypes.Len() >= 2)
			OrigininalParameterTypes.RemoveAt(0, 2);
	}

	if(BlueprintName.EndsWith("_"))
		BlueprintName.RemoveAt(BlueprintName.Len() - 1);

	// deterimine if this is an overloaded blueprint function
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

void FAIDUserMethods::ImportFromJson(const TArray<TSharedPtr<FJsonValue>>* Json)
{
	ScriptMethods.Reset(Json ? Json->Num() : 0);

	if(!Json)
		return;

	TSet<FString> OverloadedMethods;

	for(const auto smJson : *Json)
	{
		const auto obj = smJson->AsObject();
		if(!obj.IsValid())
			continue;

		FAIDScriptMethod sm;
		sm.ImportFromJson(obj, OverloadedMethods);
		ScriptMethods.Add(sm);
	}

	// mark overloaded methods
	for (auto& scriptMethod : ScriptMethods)
		scriptMethod.bIsOverloadedFunction = OverloadedMethods.Contains(scriptMethod.Name);
}

//---------------------------------------------------------------------------//

UADIHierarchyObject* UADIHierarchyObject::CreateFromJson(UObject* Outer, const TSharedPtr<FJsonObject> JsonObject)
{
	if(!JsonObject.IsValid())
		return nullptr;

	//extract properties
	FString Id;
	JSON_TRY_STRING(JsonObject, Id);
	FString TechnicalName;
	JSON_TRY_STRING(JsonObject, TechnicalName);
	FString Type;
	JSON_TRY_STRING(JsonObject, Type);

	//create new object, referenced by Outer (otherwise it is transient!), with name TechnicalName
	auto obj = NewObject<UADIHierarchyObject>(Outer, *TechnicalName);
	obj->Id = Id;
	obj->TechnicalName = TechnicalName;
	obj->Type = Type;

	//fill in children
	const TArray<TSharedPtr<FJsonValue>>* jsonChildren;
	if(!JsonObject->TryGetArrayField(TEXT("Children"), jsonChildren) && jsonChildren)
	{
		obj->Children.Reset(jsonChildren->Num());
		for(auto jsonChild : *jsonChildren)
		{
			auto child = CreateFromJson(obj, jsonChild->AsObject());
			obj->Children.Add(child);
		}
	}

	return obj;
}

void FADIHierarchy::ImportFromJson(UArticyImportData* ImportData, const TSharedPtr<FJsonObject> Json)
{
	RootObject = nullptr;

	//find the "Hierarchy" section
	if(!Json.IsValid())
		return;

	RootObject = UADIHierarchyObject::CreateFromJson(ImportData, Json);
}

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void UArticyImportData::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITORONLY_DATA
	if(!HasAnyFlags(RF_ClassDefaultObject))
	{
		ImportData = NewObject<UAssetImportData>(this, TEXT("ImportData"));
	}
#endif
}

#if WITH_EDITORONLY_DATA
void UArticyImportData::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	if(ImportData)
	{
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), ImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden));
	}

	Super::GetAssetRegistryTags(OutTags);
}
#endif

void UArticyImportData::PostImport()
{
	// update the internal save state of the package settings (add settings for new packages, remove outdated package settings, restore previous settings for still existing packages)
	UArticyPluginSettings* settings = GetMutableDefault<UArticyPluginSettings>();
	settings->UpdatePackageSettings();

	FArticyEditorModule& ArticyEditorModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>("ArticyEditor");
	ArticyEditorModule.OnImportFinished.Broadcast();
}

void UArticyImportData::ImportFromJson(const TSharedPtr<FJsonObject> RootObject)
{
	if(CachedData)
	{
		CachedData->ConditionalBeginDestroy();
	}
	
	CachedData = DuplicateObject<UArticyImportData>(this, this, FName(TEXT("CachedArticyImportData")));
	
	// import the main sections
	Settings.ImportFromJson(RootObject->GetObjectField(JSON_SECTION_SETTINGS));
	Project.ImportFromJson(RootObject->GetObjectField(JSON_SECTION_PROJECT));
	PackageDefs.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_PACKAGES));
	Hierarchy.ImportFromJson(this, RootObject->GetObjectField(JSON_SECTION_HIERARCHY));
	UserMethods.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_SCRIPTMEETHODS));

	bool bNeedsCodeGeneration = false;
	
	// import GVs and ObjectDefs only if needed
	if(Settings.DidObjectDefsOrGVsChange())
	{
		GlobalVariables.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_GLOBALVARS), this);
		ObjectDefinitions.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_OBJECTDEFS), this);
		bNeedsCodeGeneration = true;
	}

	if(Settings.DidScriptFragmentsChange() && this->GetSettings().set_UseScriptSupport)
	{
		this->GatherScripts();
		bNeedsCodeGeneration = true;
	}
	//===================================//

	// if we are generating code, generate and compile it; after it has finished, generate assets and perform post import logic
	if(bNeedsCodeGeneration)
	{
		const bool bAnyCodeGenerated = CodeGenerator::GenerateCode(this);

		if (bAnyCodeGenerated)
		{
			static FDelegateHandle PostImportHandle;

			if(PostImportHandle.IsValid())
			{
				FArticyEditorModule::Get().OnCompilationFinished.Remove(PostImportHandle);
				PostImportHandle.Reset();
			}
			
			// this will have either the current import data or the cached version
			PostImportHandle = FArticyEditorModule::Get().OnCompilationFinished.AddLambda([this](UArticyImportData* Data)
			{
				CodeGenerator::GenerateAssets(Data);
				UArticyImportData::PostImport();
			});

			CodeGenerator::Recompile(this);
		}
	}
	// if we are importing but no code needed to be generated, generate assets immediately and perform post import
	else
	{
		CodeGenerator::GenerateAssets(this);
		UArticyImportData::PostImport();
	}
}

void UArticyImportData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostInitProperties();

	UArticyImportData* UsedImportData = this;
	
	if(bForceCompleteReimport || bReimportChanges)
	{
		if(bForceCompleteReimport)
		{
			Settings.ObjectDefinitionsHash.Reset();
			Settings.ScriptFragmentsHash.Reset();
		}

		bForceCompleteReimport = bReimportChanges = bRegenerateAssets = false;

		const auto factory = NewObject<UArticyJSONFactory>();
		if(factory)
		{
			factory->Reimport(this);
			//GC will destroy factory
		}
	}
	if(bRegenerateAssets)
	{
		bRegenerateAssets = false;

		CodeGenerator::GenerateAssets(this);
	}
}

TArray<UArticyPackage*> UArticyImportData::GetPackagesDirect()
{
	TArray<UArticyPackage*> Packages;

	for (auto& Package : ImportedPackages)
	{
		Packages.Add(Package.Get());
	}
	return Packages;
}

void UArticyImportData::GatherScripts()
{
	ScriptFragments.Empty();
	PackageDefs.GatherScripts(this);
}

void UArticyImportData::AddScriptFragment(const FString& Fragment, const bool bIsInstruction)
{
	//match any group of two words separated by a dot, that does not start with a double quote
	// (?<!["a-zA-Z])(\w+\.\w+)
	//NOTE: static is no good here! crashes on application quit...
	const FRegexPattern unquotedWordDotWord(TEXT("(?<![\"a-zA-Z_])([a-zA-Z_]{1}\\w+\\.\\w+)"));
	//match an assignment operator (an = sign that does not have any of [ = < > ] before it, and no = after it)
	const FRegexPattern assignmentOperator(TEXT("(?<![=<>])=(?!=)"));

	bool bCreateBlueprintableUserMethods = UArticyPluginSettings::Get()->bCreateBlueprintTypeForScriptMethods;

	auto string = Fragment; //Fragment.Replace(TEXT("\n"), TEXT(""));
	if(string.Len() > 0)
	{
		static TArray<FString> lines; lines.Reset();
		//split into lines
		string.ParseIntoArray(lines, TEXT("\n"));

		string = TEXT("");
		FString comments = TEXT("");
		for(auto line : lines)
		{
			//remove comment
			//NOTE: this breaks once // is allowed in a string (i.e. in an object name)
			auto doubleSlashPos = line.Find(TEXT("//"));
			if(doubleSlashPos != INDEX_NONE)
			{
				comments += line.Mid(doubleSlashPos) + TEXT("\n");
				line = line.Left(doubleSlashPos);
			}

			//re-compose lines
			string += line + TEXT(" ");
		}

		//now, split at semicolons, i.e. into statements
		string.TrimEndInline();
		string.ParseIntoArray(lines, TEXT(";"));

		//a script condition must not have more than one statement (semicolon)!
		ensure(bIsInstruction || lines.Num() <= 1);

		//re-assemble the string, putting all comments at the top
		string = comments;
		for(auto l = 0; l < lines.Num(); ++l)
		{
			auto line = lines[l];

			//find all GV accesses (Namespace.Variable)
			FRegexMatcher gvAccess(unquotedWordDotWord, line);

			//find the last assignment operator in the line
			auto assignments = FRegexMatcher(assignmentOperator, line);
			auto lastAssignment = line.Len();
			while(assignments.FindNext())
				lastAssignment = assignments.GetMatchBeginning();

			//replace all remaining Namespace.Variable with *Namespace->Variable
			//note: if the variable appears to the right of an assignment operator,
			//write Namespace->Variable->Get() instead
			auto offset = 0;
			while(gvAccess.FindNext())
			{
				auto start = gvAccess.GetMatchBeginning() + offset;
				auto end = gvAccess.GetMatchEnding() + offset;

				if(lastAssignment < start)
				{
					//there is an assignment operator to the left of this, thus get the raw value
					line = line.Left(start) + line.Mid(start, end - start).Replace(TEXT("."), TEXT("->")) +
						TEXT("->Get()") + line.Mid(end);

					offset += strlen(">") + strlen("->Get()");
				}
				else
				{			
					// if the value the variable should get assigned is a string, we cast it to FString
					auto valueStr = line.Mid(end);
					for (int i = 0; i < valueStr.Len(); i++)
					{
						auto currentChar = valueStr[i];
						if (currentChar == TEXT(' ') || currentChar == TEXT('<') || currentChar == TEXT('>') || currentChar == TEXT('=') || currentChar == TEXT('+') || currentChar == TEXT('-'))
							continue;
						else if (currentChar == '\"' && i > 0)
						{
							valueStr.InsertAt(i, TEXT("(FString)"));
							offset += strlen("(FString)");
						}

						break;
					}
					
					//get the dereferenced variable
					line = line.Left(start) + TEXT("(*") + line.Mid(start, end - start).Replace(TEXT("."), TEXT("->")) + ")" + valueStr;
					offset += strlen(".") + strlen(">") + strlen("()");
				}
			}

			//re-compose the string
			string += line;

			//script conditions don't have semicolons!
			if(bIsInstruction)
				string += TEXT(";");

			//the last statement does not need a newline
			if(l < lines.Num() - 1)
				string += TEXT("\n");
		}

	}

	FArticyExpressoFragment frag;
	frag.bIsInstruction = bIsInstruction;
	frag.OriginalFragment = *Fragment;
	frag.ParsedFragment = string;
	ScriptFragments.Add(frag);
}

void UArticyImportData::AddChildToParentCache(const FArticyId Parent, const FArticyId Child)
{
	// Changed because of the way Map.Find works. In original version there were cases, when first element wasn't added because children was declared out of the scope.
	// It lead to situation when first element of children array wasn't added and for example Codex Locations weren't properly initialized
	auto& childrenRef = ParentChildrenCache.FindOrAdd(Parent);
	childrenRef.Values.AddUnique(Child);
}

void UArticyImportData::ResolveCachedVersion()
{
	ensure(CachedData != nullptr);
	
	this->Settings = CachedData->Settings;
	this->Project = CachedData->Project;
	this->GlobalVariables = CachedData->GlobalVariables;
	this->ObjectDefinitions = CachedData->ObjectDefinitions;
	this->PackageDefs = CachedData->PackageDefs;
	this->UserMethods = CachedData->UserMethods;
	this->Hierarchy = CachedData->Hierarchy;
	
	this->ScriptFragments = CachedData->ScriptFragments;

	this->ImportedPackages = CachedData->ImportedPackages;

	this->ParentChildrenCache = CachedData->ParentChildrenCache;

	this->CachedData->ConditionalBeginDestroy();
}

#undef LOCTEXT_NAMESPACE