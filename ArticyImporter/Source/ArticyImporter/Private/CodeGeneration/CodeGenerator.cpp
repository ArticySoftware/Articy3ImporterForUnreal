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
#include <IProjectManager.h>
#include <GameProjectGenerationModule.h>
#include <UnrealEdMisc.h>
#include <GenericPlatformMisc.h>
#include <Dialogs.h>
#include "../Launch/Resources/Version.h"
#include "ArticyImporter.h"

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------// 
#define LOCTEXT_NAMESPACE "CodeGenerator"

FString CodeGenerator::GetSourceFolder()
{
	return FPaths::GameSourceDir() / FApp::GetProjectName() / TEXT("ArticyGenerated");
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

void CodeGenerator::GenerateCode(UArticyImportData* Data)
{
	if (!Data)
		return;

	//generate GVs and ObjectDefinitions only if needed
	if (Data->GetSettings().DidObjectDefsOrGVsChange())
	{
		//DeleteGeneratedCode();

		GlobalVarsGenerator::GenerateCode(Data);
		DatabaseGenerator::GenerateCode(Data);
		InterfacesGenerator::GenerateCode(Data);
		ObjectDefinitionsGenerator::GenerateCode(Data);
	}

	//gather all the scripts in all packages
	if (Data->GetSettings().set_UseScriptSupport)
		Data->GatherScripts();

	//generate the expresso class, containing all the c++ script fragments
	//the class is also generated if script support is disabled, it's just empty in that case
	ExpressoScriptsGenerator::GenerateCode(Data);

	//trigger compile
	Compile(Data);
}

void CodeGenerator::Recompile(UArticyImportData* Data)
{
	Compile(Data);
}

bool CodeGenerator::DeleteGeneratedAssets()
{
	return FPlatformFileManager::Get().GetPlatformFile().DeleteDirectoryRecursively(*ArticyHelpers::ArticyGeneratedFolder);
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

	static FDelegateHandle lambdaHandle;
	if (lambdaHandle.IsValid())
		IHotReloadModule::Get().OnModuleCompilerFinished().Remove(lambdaHandle);

	lambdaHandle = IHotReloadModule::Get().OnModuleCompilerFinished().AddLambda([=](FString OutputLog, ECompilationResult::Type Result, bool bShowLog)
	{
		OnCompiled(Result, Data, bWaitingForOtherCompile);
	});

	if (!bWaitingForOtherCompile)
		HotReloadSupport.DoHotReloadFromEditor(EHotReloadFlags::None /*async*/);
}

void CodeGenerator::OnCompiled(const ECompilationResult::Type Result, UArticyImportData* Data, const bool bWaitingForOtherCompile)
{
	bool succeeded = Result == ECompilationResult::Succeeded || Result == ECompilationResult::UpToDate;
	if (!succeeded)
	{
		//compile failed
		UE_LOG(LogArticyImporter, Error, TEXT("Compile failed, cannot continue importing articy:draft data!"));
		return;
	}

	if (bWaitingForOtherCompile || Data->GetSettings().DidObjectDefsOrGVsChange())
	{
		if (!bWaitingForOtherCompile)
		{
			//the object definitions are up to date now
			Data->GetSettings().SetObjectDefinitionsRebuilt();
		}

		//another compile is needed
		Compile(Data);
		return;
	}

	//compiling is done!
	//check if UArticyBaseGlobalVariables can be found, otherwise something went wrong!
	auto className = GetGlobalVarsClassname(Data, true);
	auto fullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), FApp::GetProjectName(), *className);
	if (!ensure(ConstructorHelpersInternal::FindOrLoadClass(fullClassName, UArticyGlobalVariables::StaticClass())))
		UE_LOG(LogArticyImporter, Error, TEXT("Could not find generated global variables class after compile!"));

	ensure(DeleteGeneratedAssets());

	// cache currently existing articy data for cleanup
	PackagesGenerator::CacheExistingArticyData(Data);

	//generate the global variables asset
	GlobalVarsGenerator::GenerateAsset(Data);
	//generate the database asset
	auto db = DatabaseGenerator::GenerateAsset(Data);
	//generate assets for all the imported objects
	PackagesGenerator::GenerateAssets(Data);

	// execute cleanup: delete old assets, rename assets to what they should be, delete old directories
	PackagesGenerator::ExecuteCleanup();

	//register the newly imported packages in the database
	if (ensureMsgf(db, TEXT("Could not create ArticyDatabase asset!")))
	{
		db->SetLoadedPackages(Data->GetPackages());
		db->MarkPackageDirty();
	}
	//prompt the user to save newly generated packages
	FEditorFileUtils::SaveDirtyPackages(true, true, /*bSaveContentPackages*/ true, false, false, true);	

	FArticyImporterModule& importerModule = FModuleManager::Get().GetModuleChecked<FArticyImporterModule>("ArticyImporter");
	importerModule.OnImportFinished.Broadcast();
}

#undef LOCTEXT_NAMESPACE