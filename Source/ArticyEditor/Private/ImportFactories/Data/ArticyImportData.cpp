//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyImportData.h"
#include "EditorFramework/AssetImportData.h"
#include "CodeGeneration/CodeGenerator.h"
#include "ArticyPluginSettings.h"
#include "Internationalization/Regex.h"
#include "ArticyEditorModule.h"
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
#else
#include "Misc/MessageDialog.h"
#endif
#include "BuildToolParser/BuildToolParser.h"
#include "Expresso/ExpressoFragment.h"
#include "GlobalVariables/GVNamespace.h"
#include "GlobalVariables/GVTypes.h"
#include "Manifest/Manifest.h"

#define LOCTEXT_NAMESPACE "ArticyImportData"

void UArticyImportData::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITORONLY_DATA
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		ImportData = NewObject<UAssetImportData>(this, TEXT("ImportData"));
	}
#endif
}

#if WITH_EDITORONLY_DATA
void UArticyImportData::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	if (ImportData)
	{
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), ImportData->GetSourceData().ToJson(),
		                              FAssetRegistryTag::TT_Hidden));
	}

	Super::GetAssetRegistryTags(OutTags);
}
#endif

void UArticyImportData::PostImport()
{
	FArticyEditorModule& ArticyEditorModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>(
		"ArticyEditor");
	ArticyEditorModule.OnImportFinished.Broadcast();
}

// Archive import.
// To be valid, the "_json*" pointers must be set.
// called from BinaryFileDispatcher if a reimport is needed.
// UArticyImportData, one class to rule them all (bis)... Q&D principles powa!
void UArticyImportData::MainImportFromDispatcher()
{
	if (_jsonManifest.IsValid())
	{
		Manifest.ImportFromJson(_jsonManifest);
		Settings.ImportFromJson(_jsonManifest->GetObjectField(JSON_SECTION_SETTINGS));
		Project.ImportFromJson(_jsonManifest->GetObjectField(JSON_SECTION_PROJECT));
	}
	if (_jsonPackageDefs.IsValid())
		PackageDefs.ImportFromJson(&_jsonPackageDefs->GetArrayField(JSON_SECTION_PACKAGES));
	if (_jsonHierarchy.IsValid())
		Hierarchy.ImportFromJson(this, _jsonHierarchy->GetObjectField(JSON_SECTION_HIERARCHY));
	if (_jsonUserMethods.IsValid())
		UserMethods.ImportFromJson(&_jsonUserMethods->GetArrayField(JSON_SECTION_SCRIPTMEETHODS));

	/// W8 4 new or changes specs frm archv...
	bool bNeedsCodeGeneration = false;

	ParentChildrenCache.Empty();

	// import GVs and ObjectDefs only if needed

	
	if (Settings.DidObjectDefsOrGVsChange())
	{
		if (_jsonGlobalVars.IsValid())
			GlobalVariables.ImportFromJson(&_jsonGlobalVars->GetArrayField(JSON_SECTION_GLOBALVARS), this);
		if (_jsonObjectDefinitions.IsValid())
			ObjectDefinitions.ImportFromJson(&_jsonObjectDefinitions->GetArrayField(JSON_SECTION_OBJECTDEFS), this);

		bNeedsCodeGeneration = true;
	}

	if (Settings.DidScriptFragmentsChange() && this->GetSettings().set_UseScriptSupport)
	{
		this->GatherScripts();
		bNeedsCodeGeneration = true;
	}
	//===================================//

	// ArticyRuntime reference check, ask user to add "ArticyRuntime" Reference to Unreal build tool if needed.
	if (UArticyPluginSettings::Get()->bVerifyArticyReferenceBeforeImport)
	{
		FString path = FPaths::GameSourceDir() / FApp::GetProjectName() / FApp::GetProjectName() + TEXT(".Build.cs");
		// TEXT("");
		BuildToolParser RefVerifier = BuildToolParser(path);
		if (!RefVerifier.VerifyArticyRuntimeRef())
		{
			const FText RuntimeRefNotFoundTitle = FText::FromString(TEXT("ArticyRuntime reference not found."));
			const FText RuntimeRefNotFound = LOCTEXT("ArticyRuntimeReferenceNotFound",
			                                         "The \"ArticyRuntime\" reference needs to be added inside the Unreal build tool.\nDo you want to add the reference automatically ?\nIf you use a custom build system or a custom build file, you can disable automatic reference verification inside the Articy Plugin settings from the Project settings.\n");
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
			EAppReturnType::Type ReturnType = OpenMsgDlgInt(EAppMsgType::Ok, RuntimeRefNotFound, RuntimeRefNotFoundTitle);
#else
			EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNoCancel, RuntimeRefNotFound,
			                                                       &RuntimeRefNotFoundTitle);
#endif
			if (ReturnType == EAppReturnType::Yes)
			{
				RefVerifier.AddArticyRuntimmeRef();
			}
			else if (ReturnType == EAppReturnType::Cancel)
			{
				// Abort code generation
				bNeedsCodeGeneration = false;
			}
		}
	}

	// if we are generating code, generate and compile it; after it has finished, generate assets and perform post import logic
	if (bNeedsCodeGeneration)
	{
		const bool bAnyCodeGenerated = CodeGenerator::GenerateCode(this);

		if (bAnyCodeGenerated)
		{
			static FDelegateHandle PostImportHandle;

			if (PostImportHandle.IsValid())
			{
				FArticyEditorModule::Get().OnCompilationFinished.Remove(PostImportHandle);
				PostImportHandle.Reset();
			}

			// this will have either the current import data or the cached version
			PostImportHandle = FArticyEditorModule::Get().OnCompilationFinished.AddLambda(
				[this](UArticyImportData* Data)
				{
					BuildCachedVersion();
					CodeGenerator::GenerateAssets(Data);
					PostImport();
				});

			CodeGenerator::Recompile(this);
		}
	}
	// if we are importing but no code needed to be generated, generate assets immediately and perform post import
	else
	{
		BuildCachedVersion();
		CodeGenerator::GenerateAssets(this);
		PostImport();
	}
}

// UArticyImportData, one class to rule them all ! (legacy version/ One big Json file import)
void UArticyImportData::MainImportFromJson(const TSharedPtr<FJsonObject> RootObject)
{
	// import the main sections
	Settings.ImportFromJson(RootObject->GetObjectField(JSON_SECTION_SETTINGS));
	Project.ImportFromJson(RootObject->GetObjectField(JSON_SECTION_PROJECT));
	PackageDefs.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_PACKAGES));
	Hierarchy.ImportFromJson(this, RootObject->GetObjectField(JSON_SECTION_HIERARCHY));
	UserMethods.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_SCRIPTMEETHODS));

	bool bNeedsCodeGeneration = false;

	ParentChildrenCache.Empty();

	// import GVs and ObjectDefs only if needed
	if (Settings.DidObjectDefsOrGVsChange())
	{
		GlobalVariables.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_GLOBALVARS), this);
		ObjectDefinitions.ImportFromJson(&RootObject->GetArrayField(JSON_SECTION_OBJECTDEFS), this);
		bNeedsCodeGeneration = true;
	}

	if (Settings.DidScriptFragmentsChange() && this->GetSettings().set_UseScriptSupport)
	{
		this->GatherScripts();
		bNeedsCodeGeneration = true;
	}
	//===================================//

	// ArticyRuntime reference check, ask user to add "ArticyRuntime" Reference to Unreal build tool if needed.
	if (UArticyPluginSettings::Get()->bVerifyArticyReferenceBeforeImport)
	{
		FString path = FPaths::GameSourceDir() / FApp::GetProjectName() / FApp::GetProjectName() + TEXT(".Build.cs");
		// TEXT("");
		BuildToolParser RefVerifier = BuildToolParser(path);
		if (!RefVerifier.VerifyArticyRuntimeRef())
		{
			const FText RuntimeRefNotFoundTitle = FText::FromString(TEXT("ArticyRuntime reference not found."));
			const FText RuntimeRefNotFound = LOCTEXT("ArticyRuntimeReferenceNotFound",
			                                         "The \"ArticyRuntime\" reference needs to be added inside the Unreal build tool.\nDo you want to add the reference automatically ?\nIf you use a custom build system or a custom build file, you can disable automatic reference verification inside the Articy Plugin settings from the Project settings.\n");
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 24
			EAppReturnType::Type ReturnType = OpenMsgDlgInt(EAppMsgType::Ok, RuntimeRefNotFound, RuntimeRefNotFoundTitle);
#else
			EAppReturnType::Type ReturnType = FMessageDialog::Open(EAppMsgType::YesNoCancel, RuntimeRefNotFound,
			                                                       &RuntimeRefNotFoundTitle);
#endif
			if (ReturnType == EAppReturnType::Yes)
			{
				RefVerifier.AddArticyRuntimmeRef();
			}
			else if (ReturnType == EAppReturnType::Cancel)
			{
				// Abort code generation
				bNeedsCodeGeneration = false;
			}
		}
	}

	// if we are generating code, generate and compile it; after it has finished, generate assets and perform post import logic
	if (bNeedsCodeGeneration)
	{
		const bool bAnyCodeGenerated = CodeGenerator::GenerateCode(this);

		if (bAnyCodeGenerated)
		{
			static FDelegateHandle PostImportHandle;

			if (PostImportHandle.IsValid())
			{
				FArticyEditorModule::Get().OnCompilationFinished.Remove(PostImportHandle);
				PostImportHandle.Reset();
			}

			// this will have either the current import data or the cached version
			PostImportHandle = FArticyEditorModule::Get().OnCompilationFinished.AddLambda(
				[this](UArticyImportData* Data)
				{
					BuildCachedVersion();
					CodeGenerator::GenerateAssets(Data);
					PostImport();
				});

			CodeGenerator::Recompile(this);
		}
	}
	// if we are importing but no code needed to be generated, generate assets immediately and perform post import
	else
	{
		BuildCachedVersion();
		CodeGenerator::GenerateAssets(this);
		PostImport();
	}
}

const TWeakObjectPtr<UArticyImportData> UArticyImportData::GetImportData()
{
	static TWeakObjectPtr<UArticyImportData> ImportData = nullptr;

	if (!ImportData.IsValid())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			"AssetRegistry");
		TArray<FAssetData> AssetData;

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0
		AssetRegistryModule.Get().
		                    GetAssetsByClass(UArticyGlobalVariables::StaticClass()->GetClassPathName(), AssetData);
#else
		AssetRegistryModule.Get().GetAssetsByClass(UArticyImportData::StaticClass()->GetFName(), AssetData);
#endif

		if (!AssetData.Num())
		{
			UE_LOG(LogArticyEditor, Warning, TEXT("Could not find articy import data asset."));
		}
		else
		{
			ImportData = Cast<UArticyImportData>(AssetData[0].GetAsset());

			if (AssetData.Num() > 1)
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >0
				UE_LOG(LogArticyEditor, Error,
			       TEXT(
				       "Found more than one import file. This is not supported by the plugin. Using the first found file for now: %s"
			       ),
			       *AssetData[0].GetObjectPathString());
#else
				UE_LOG(LogArticyEditor, Error,
			       TEXT(
				       "Found more than one import file. This is not supported by the plugin. Using the first found file for now: %s"
			       ),
			       *AssetData[0].ObjectPath.ToString());
#endif
		}
	}

	return ImportData;
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

	// regex pattern to find literal string, even if they contain escaped quotes (looks nasty if string escaped...): "([^"\\]|\\[\s\S])*" 
	const FRegexPattern literalStringPattern(TEXT("\"([^\"\\\\]|\\\\[\\s\\S])*\""));

	bool bCreateBlueprintableUserMethods = UArticyPluginSettings::Get()->bCreateBlueprintTypeForScriptMethods;

	auto string = Fragment; //Fragment.Replace(TEXT("\n"), TEXT(""));
	if (string.Len() > 0)
	{
		static TArray<FString> lines;
		lines.Reset();
		//split into lines
		string.ParseIntoArray(lines, TEXT("\n"));

		string = TEXT("");
		FString comments = TEXT("");
		for (auto line : lines)
		{
			//remove comment
			//NOTE: this breaks once // is allowed in a string (i.e. in an object name)
			auto doubleSlashPos = line.Find(TEXT("//"));
			if (doubleSlashPos != INDEX_NONE)
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
		for (auto l = 0; l < lines.Num(); ++l)
		{
			auto line = lines[l];

			// since "line" gets modified after the literalStrings matcher was created
			// we need to offset the values from the matcher based on the changes done to "line" in the loop
			auto offset = 0;

			// create FStrings from literal strings
			FRegexMatcher literalStrings(literalStringPattern, line);
			while (literalStrings.FindNext())
			{
				auto literalStart = literalStrings.GetMatchBeginning() + offset;
				auto literalEnd = literalStrings.GetMatchEnding() + offset;

				line = line.Left(literalStart) + TEXT("FString(TEXT(") + line.Mid(
					literalStart, literalEnd - literalStart) + TEXT("))") + line.Mid(literalEnd);
				offset += strlen("FString(TEXT(") + strlen("))");
			}

			//find all GV accesses (Namespace.Variable)
			FRegexMatcher gvAccess(unquotedWordDotWord, line);

			//find the last assignment operator in the line
			auto assignments = FRegexMatcher(assignmentOperator, line);
			auto lastAssignment = line.Len();
			while (assignments.FindNext())
				lastAssignment = assignments.GetMatchBeginning();

			offset = 0;

			//replace all remaining Namespace.Variable with *Namespace->Variable
			//note: if the variable appears to the right of an assignment operator,
			//write Namespace->Variable->Get() instead
			while (gvAccess.FindNext())
			{
				auto start = gvAccess.GetMatchBeginning() + offset;
				auto end = gvAccess.GetMatchEnding() + offset;

				literalStrings = FRegexMatcher(literalStringPattern, line);
				auto inLiteral = false;
				while (literalStrings.FindNext())
				{
					// no offset, since this line copy is the current up-to-date one
					auto literalStart = literalStrings.GetMatchBeginning();
					auto literalEnd = literalStrings.GetMatchEnding();

					if (start >= literalStart && end <= literalEnd)
					{
						inLiteral = true;
						break;
					}
				}

				if (!inLiteral)
				{
					bool bDoCast = false;
					// only to GV replacement if we are not within a literal string
					if (lastAssignment < start)
					{
						if (!bIsInstruction)
						{
							if (IsVariableOfType(EArticyType::ADT_Integer, line.Mid(start, end - start)))
							{
								bDoCast = true;
							}
						}
						// only to GV replacement if we are not within a literal string
						//there is an assignment operator to the left of this, thus get the raw value
						if (!bIsInstruction && bDoCast)
						{
							line = line.Left(start) + TEXT("(int)(") + line.Mid(start, end - start).Replace(
									TEXT("."), TEXT("->")) +
								TEXT("->Get())") + line.Mid(end);

							offset += strlen("(int)(>") + strlen("->Get())");
						}
						else
						{
							if (!bIsInstruction)
							{
								if (IsVariableOfType(EArticyType::ADT_Integer, line.Mid(start, end - start)))
								{
									bDoCast = true;
								}
							}

							//get the dereferenced variable
							if (!bIsInstruction && bDoCast)
							{
								line = line.Left(start) + TEXT("(int)(*") + line.Mid(start, end - start).Replace(
									TEXT("."), TEXT("->")) + ")" + line.Mid(end);

								offset += strlen("(int)(*") + strlen(">") + strlen(")");
							}
							else
							{
								line = line.Left(start) + TEXT("(*") + line.Mid(start, end - start).Replace(
									TEXT("."), TEXT("->")) + ")" + line.Mid(end);

								offset += strlen(".") + strlen(">") + strlen("()");
							}
						}
					}
					else
					{
						//get the dereferenced variable
						line = line.Left(start) + TEXT("(*") + line.Mid(start, end - start).Replace(
							TEXT("."), TEXT("->")) + ")" + line.Mid(end);
						offset += strlen(".") + strlen(">") + strlen("()");
					}
				} // !inLiteral
			} // GV matching

			//re-compose the string
			string += line;

			//script conditions don't have semicolons!
			if (bIsInstruction)
				string += TEXT(";");

			//the last statement does not need a newline
			if (l < lines.Num() - 1)
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

void UArticyImportData::BuildCachedVersion()
{
	CachedData.Settings = this->Settings;
	CachedData.Project = this->Project;
	CachedData.GlobalVariables = this->GlobalVariables;
	CachedData.ObjectDefinitions = this->ObjectDefinitions;
	CachedData.PackageDefs = this->PackageDefs;
	CachedData.UserMethods = this->UserMethods;
	CachedData.Hierarchy = this->Hierarchy;
	CachedData.ScriptFragments = this->ScriptFragments;
	CachedData.ImportedPackages = this->ImportedPackages;
	CachedData.ParentChildrenCache = this->ParentChildrenCache;
}

void UArticyImportData::ResolveCachedVersion()
{
	ensure(HasCachedVersion());

	this->Settings = CachedData.Settings;
	this->Project = CachedData.Project;
	this->GlobalVariables = CachedData.GlobalVariables;
	this->ObjectDefinitions = CachedData.ObjectDefinitions;
	this->PackageDefs = CachedData.PackageDefs;
	this->UserMethods = CachedData.UserMethods;
	this->Hierarchy = CachedData.Hierarchy;
	this->ScriptFragments = CachedData.ScriptFragments;
	this->ImportedPackages = CachedData.ImportedPackages;
	this->ParentChildrenCache = CachedData.ParentChildrenCache;
	this->CachedData = FArticyImportDataStruct();
	this->bHasCachedVersion = false;
}

bool UArticyImportData::IsVariableOfType(EArticyType varType, FString FullName)
{
	FString GVNameSpace, GVVariable;
	FullName.Split(TEXT("."), &GVNameSpace, &GVVariable);

	for (auto Nmspc : GetGlobalVars().Namespaces)
	{
		if (Nmspc.Namespace.Compare(GVNameSpace) == 0)
		{
			for (auto var : Nmspc.Variables)
			{
				if (var.Variable.Compare(GVVariable) == 0)
				{
					if (var.Type == varType)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}
#undef LOCTEXT_NAMESPACE
