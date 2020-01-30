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
#include "UnrealEdMisc.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "ArticyImporter.h"
#include "ArticyPluginSettings.h"
#include "IContentBrowserSingleton.h"

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

bool CodeGenerator::GenerateCode(UArticyImportData* Data)
{
	if (!Data)
		return false;

	bool bCodeGenerated = false;
	
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

	static FDelegateHandle lambdaHandle;
	if (lambdaHandle.IsValid())
		IHotReloadModule::Get().OnHotReload().Remove(lambdaHandle);

	lambdaHandle = IHotReloadModule::Get().OnHotReload().AddLambda([=](bool bWasTriggeredAutomatically)
	{
		OnCompiled(Data, bWaitingForOtherCompile);
	});
	
	if (!bWaitingForOtherCompile)
		HotReloadSupport.DoHotReloadFromEditor(EHotReloadFlags::None /*async*/);
}

void CodeGenerator::GenerateAssets(UArticyImportData* Data)
{
	//compiling is done!
	//check if UArticyBaseGlobalVariables can be found, otherwise something went wrong!
	auto className = GetGlobalVarsClassname(Data, true);
	auto fullClassName = FString::Printf(TEXT("Class'/Script/%s.%s'"), FApp::GetProjectName(), *className);
	if (!ensure(ConstructorHelpersInternal::FindOrLoadClass(fullClassName, UArticyGlobalVariables::StaticClass())))
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
		db->SetLoadedPackages(Data->GetPackages());
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

void CodeGenerator::OnCompiled(UArticyImportData* Data, const bool bWaitingForOtherCompile)
{
	if(!bWaitingForOtherCompile)
	{
		Data->GetSettings().SetObjectDefinitionsRebuilt();
		Data->GetSettings().SetScriptFragmentsRebuilt();
		// broadcast that compilation has finished. ArticyImportData will then generate the assets and perform post import operations
		FArticyImporterModule::Get().OnCompilationFinished.Broadcast();
	}
	else
	{
		// if we were waiting for another compile, that means our generated code hasn't been compiled yet. Trigger another compile
		Compile(Data);
	}
}

#undef LOCTEXT_NAMESPACE