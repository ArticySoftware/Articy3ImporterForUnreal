//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "CodeGenerator.h"
#include "ArticyImportData.h"
#include "DatabaseGenerator.h"
#include "GlobalVarsGenerator.h"
#include "InterfacesGenerator.h"
#include "Developer/HotReload/Public/IHotReload.h"
#include "ArticyRuntime/Public/ArticyGlobalVariables.h"
#include "ObjectDefinitionsGenerator.h"
#include "PackagesGenerator.h"
#include "ArticyDatabase.h"
#include "ExpressoScriptsGenerator.h"
#include "FileHelpers.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "ArticyImporter.h"
#include "ArticyPluginSettings.h"
#include "Dialogs/Dialogs.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/IPluginManager.h"
#include "ObjectTools.h"

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------// 
#define LOCTEXT_NAMESPACE "CodeGenerator"

TMap<FString, FString> CodeGenerator::CachedFiles;

FString CodeGenerator::GetSourceFolder()
{
	FString GeneratedFilesSourceFolder = IPluginManager::Get().FindPlugin(TEXT("ArticyImporter"))->GetBaseDir() / TEXT("Source") / TEXT("ArticyGenerated") / TEXT("ArticyGenerated");
	return GeneratedFilesSourceFolder;
}

FString CodeGenerator::GetGeneratedInterfacesFilename(const UArticyImportData* Data)
{
	return Data->GetProject().TechnicalName + "Interfaces";
}

FString CodeGenerator::GetGeneratedTypesFilename(const UArticyImportData* Data)
{
	return Data->GetProject().TechnicalName + "ArticyTypes";
}

FString CodeGenerator::GetGlobalVarsClassname(const UArticyImportData* Data, const bool bOmittPrefix)
{
	return (bOmittPrefix ? "" : "U") + Data->GetProject().TechnicalName + "GlobalVariables";
}

FString CodeGenerator::GetGVNamespaceClassname(const UArticyImportData* Data, const FString& Namespace)
{
	return "U" + Data->GetProject().TechnicalName + Namespace + "Variables";
}

FString CodeGenerator::GetDatabaseClassname(const UArticyImportData* Data, const bool bOmittPrefix)
{
	return (bOmittPrefix ? "" : "U") + Data->GetProject().TechnicalName + "Database";
}

FString CodeGenerator::GetMethodsProviderClassname(const UArticyImportData* Data, const bool bOmittPrefix)
{
	return (bOmittPrefix ? "" : "U") + Data->GetProject().TechnicalName + "MethodsProvider";
}

FString CodeGenerator::GetExpressoScriptsClassname(const UArticyImportData* Data, const bool bOmittPrefix)
{
	return (bOmittPrefix ? "" : "U") + Data->GetProject().TechnicalName + "ExpressoScripts";
}

FString CodeGenerator::GetFeatureInterfaceClassName(const UArticyImportData* Data, const FArticyTemplateFeatureDef& Feature, const bool bOmittPrefix)
{
	return (bOmittPrefix ? "" : "I") + Data->GetProject().TechnicalName + "ObjectWith" + Feature.GetTechnicalName() + "Feature";
}

bool CodeGenerator::DeleteGeneratedCode(const FString& Filename)
{
	if (Filename.IsEmpty())
		return FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*GetSourceFolder());

	return FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*(GetSourceFolder() / Filename));
}

bool CodeGenerator::GenerateCode(UArticyImportData* Data)
{
	if (!Data)
		return false;

	bool bCodeGenerated = false;

	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *GetSourceFolder());
	TArray<FString> FileContents;
	FileContents.SetNumZeroed(FileNames.Num());

	for (int32 i = 0; i < FileNames.Num(); i++)
	{
		FileNames[i] = GetSourceFolder() / FileNames[i];
	}
	
	for(int32 i=0; i < FileNames.Num(); i++)
	{
		FFileHelper::LoadFileToString(FileContents[i], *FileNames[i]);
		CachedFiles.Add(FileNames[i], FileContents[i]);
	}


	//generate all files if ObjectDefs or GVs changed
	if (Data->GetSettings().DidObjectDefsOrGVsChange())
	{
		//DeleteGeneratedCode();

		GlobalVarsGenerator::GenerateCode(Data);
		DatabaseGenerator::GenerateCode(Data);
		InterfacesGenerator::GenerateCode(Data);
		ObjectDefinitionsGenerator::GenerateCode(Data);
		/* generate scripts as well due to them including the generated global variables
		 * if we remove a GV set but don't regenerate expresso scripts, the resulting class won't compile */
		ExpressoScriptsGenerator::GenerateCode(Data);
		bCodeGenerated = true;
	}
	// if object defs of GVs didn't change, but scripts changed, regenerate only expresso scripts
	else if (Data->GetSettings().DidScriptFragmentsChange())
	{
		ExpressoScriptsGenerator::GenerateCode(Data);
		bCodeGenerated = true;
	}

	return bCodeGenerated;
}

void CodeGenerator::Recompile(UArticyImportData* Data)
{
	Compile(Data);
}

bool CodeGenerator::DeleteGeneratedAssets()
{
	FAssetRegistryModule& AssetRegistry = FModuleManager::Get().GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> OutAssets;
	AssetRegistry.Get().GetAssetsByPath(FName(*ArticyHelpers::ArticyGeneratedFolder), OutAssets, true, false);

	TArray<UObject*> ExistingAssets;
	
	for(FAssetData data : OutAssets)
	{
		ExistingAssets.Add(data.GetAsset());
	}

	if(ExistingAssets.Num() > 0)
	{
		return ObjectTools::ForceDeleteObjects(ExistingAssets, false) > 0;
	}

	// returns true if there is nothing to delete to not trigger the ensure
	return true;
}

void CodeGenerator::Compile(UArticyImportData* Data)
{
	bool bWaitingForOtherCompile = false;

	// We can only hot-reload via DoHotReloadFromEditor when we already had code in our project
	IHotReloadInterface& HotReloadSupport = FModuleManager::LoadModuleChecked<IHotReloadInterface>("HotReload");
	if (HotReloadSupport.IsCurrentlyCompiling())
	{
		bWaitingForOtherCompile = true;
		UE_LOG(LogArticyImporter, Warning, TEXT("Already compiling, waiting until it's done."));
	}

	static FDelegateHandle AfterCompileLambda;
	if(AfterCompileLambda.IsValid())
	{
		IHotReloadModule::Get().OnHotReload().Remove(AfterCompileLambda);
		AfterCompileLambda.Reset();
	}
	
	AfterCompileLambda = IHotReloadModule::Get().OnHotReload().AddLambda([=](bool bWasTriggeredAutomatically)
	{
		UE_LOG(LogArticyImporter, Warning, TEXT("AfterCompileLamba Called"));
		OnCompiled(Data);
	});

	// register a lambda to handle failure in code generation (compilation failed due to generated articy code)
	// detection of faulty articy code is a heuristic and not optimal!
	static FDelegateHandle RestoreCachedVersionHandle;
	if(RestoreCachedVersionHandle.IsValid())
	{
		IHotReloadModule::Get().OnModuleCompilerFinished().Remove(RestoreCachedVersionHandle);
		RestoreCachedVersionHandle.Reset();
	}
	RestoreCachedVersionHandle = IHotReloadModule::Get().OnModuleCompilerFinished().AddLambda([=](const FString& Log, ECompilationResult::Type Type, bool bSuccess)
	{
		UE_LOG(LogArticyImporter, Warning, TEXT("Restore Cached Lamba Called"));

		if (Type == ECompilationResult::Succeeded || Type == ECompilationResult::Canceled || Type == ECompilationResult::UpToDate)
		{
			// do nothing in case the compilation was successful or intentionally cancelled
		}
		else
		{				
			TArray<FString> Lines;
			// parsing into individual FStrings for each line. Using \n as delimiter, should cover both Mac OSX and Windows
			Log.ParseIntoArray(Lines, TEXT("\n"));

			// heuristic: error due to articy?
			bool bErrorInGeneratedCode = false;
			for (const FString& Line : Lines)
			{
				if (Line.Contains(TEXT("error")) && Line.Contains(TEXT("ArticyGenerated")))
				{
					bErrorInGeneratedCode = true;
					break;
				}
			}

			/** if error is due to articy, restore the previous data, consisting of:
			* ImportData (base truth for generation of assets and code),
			* Generated code (previous code should have been working, so we'll restore that instead of just generating the code again)
			* Assets (we can restore the assets by generating them normally, because the ImportData was restored and the code is the same as before)
			*/
			if (bErrorInGeneratedCode)
			{
				// transfer the cached data into the current one
				Data->ResolveCachedVersion();

				// attempt to restore all generated files
				const bool bFilesRestored = RestoreCachedFiles();

				// if we succeeded, tell the user and call OnCompiled - which will then create the assets
				if(bFilesRestored)
				{
					const FText CacheRestoredText = LOCTEXT("ImportDataCacheRestoredText", "Error in generated articy code detected. Previous data restored. Proceeding with asset generation with the restored data.");
					const FText CacheRestoredTitle = LOCTEXT("ImportDataCacheRestoredTitle", "Articy code generation failed - restoration succeeded");
					OpenMsgDlgInt(EAppMsgType::Ok, CacheRestoredText, CacheRestoredTitle);
					// although we didn't actually compile, we act as if we did to trigger asset generation and post import
					OnCompiled(Data);
				}
				else
				{
					// if there were no previous files or not all files could be restored, delete them instead
					if(DeleteGeneratedCode())
					{
						const FText CacheDeletedText = LOCTEXT("ImportDataCacheDeletedText", "Error in generated articy code detected. Code deleted.");
						const FText CacheDeletedTitle = LOCTEXT("ImportDataCacheDeletedTitle", "Articy code generation failed - restoration succeeded");
						OpenMsgDlgInt(EAppMsgType::Ok, CacheDeletedText, CacheDeletedTitle);
					}
					// if deletion didn't work for some reason, notify the user
					else
					{
						const FText CacheDeletionFailedText = LOCTEXT("ImportDataCacheDeletionFailedText", "Error in generated articy code detected. Code could not be deleted.");
						const FText CacheDeletionFailedTitle = LOCTEXT("ImportDataCacheDeletionFailedTitle", "Articy code generation failed - restoration failed");
						OpenMsgDlgInt(EAppMsgType::Ok, CacheDeletionFailedText, CacheDeletionFailedTitle);
					}
				}
			}
		}
	});
	
	if (!bWaitingForOtherCompile)
	{
		HotReloadSupport.DoHotReloadFromEditor(EHotReloadFlags::None /*async*/);
		//IHotReloadModule::Get().RecompileModule(FName(TEXT("ArticyGenerated")), )
	}
}

void CodeGenerator::GenerateAssets(UArticyImportData* Data)
{
	ensure(Data);
	
	//compiling is done!
	//check if UArticyBaseGlobalVariables can be found, otherwise something went wrong!
	const auto ClassName = GetGlobalVarsClassname(Data, true);
	auto FullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), TEXT("ArticyGenerated"), *ClassName);
	if (!ensure(ConstructorHelpersInternal::FindOrLoadClass(FullClassName, UArticyGlobalVariables::StaticClass())))
	UE_LOG(LogArticyImporter, Error, TEXT("Could not find generated global variables class after compile!"));

	ensure(DeleteGeneratedAssets());

	//generate the global variables asset
	GlobalVarsGenerator::GenerateAsset(Data);
	//generate the database asset
	auto db = DatabaseGenerator::GenerateAsset(Data);
	//generate assets for all the imported objects
	PackagesGenerator::GenerateAssets(Data);

	//register the newly imported packages in the database
	if (ensureMsgf(db, TEXT("Could not create ArticyDatabase asset!")))
	{
		db->SetLoadedPackages(Data->GetPackagesDirect());
	}

	//gather all articy assets to save them
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> GeneratedAssets;
	AssetRegistryModule.Get().GetAssetsByPath(FName(*ArticyHelpers::ArticyGeneratedFolder), GeneratedAssets, true);

	TArray<UPackage*> PackagesToSave;

	PackagesToSave.Add(Data->GetOutermost());
	for (FAssetData AssetData : GeneratedAssets)
	{
		PackagesToSave.Add(AssetData.GetAsset()->GetOutermost());
	}

	// automatically save all articy assets
	TArray<UPackage*> FailedToSavePackages;
	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false, &FailedToSavePackages);

	for (auto Package : FailedToSavePackages)
	{
		UE_LOG(LogArticyImporter, Error, TEXT("Could not save package %s"), *Package->GetName());
	}
}

void CodeGenerator::OnCompiled(UArticyImportData* Data)
{
	Data->GetSettings().SetObjectDefinitionsRebuilt();
	Data->GetSettings().SetScriptFragmentsRebuilt();
	// broadcast that compilation has finished. ArticyImportData will then generate the assets and perform post import operations
	FArticyImporterModule::Get().OnCompilationFinished.Broadcast(Data);
}

bool CodeGenerator::RestoreCachedFiles()
{
	bool bFilesRestored = CachedFiles.Num() > 0 ? true : false;

	for(auto& CachedFile : CachedFiles)
	{
		bFilesRestored = bFilesRestored && FFileHelper::SaveStringToFile(CachedFile.Value, *CachedFile.Key);
	}

	return bFilesRestored;
}

#undef LOCTEXT_NAMESPACE