//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ADJsonFactory.h"
#include "ADAssetsBuilders/FileDispatcher/JsonFileDispatcher.h"
#include "Data/ArticyImportData.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "ArticyEditor/Public/ArticyEditorModule.h"

UADJsonFactory::UADJsonFactory()
{
	bEditorImport = true;
	Formats.Add(TEXT("articyue4;Articy:Draft exported file"));
}

UClass* UADJsonFactory::ResolveSupportedClass()
{
	return UArticyImportData::StaticClass();
}

bool UADJsonFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(LogArticyEditor, Log, TEXT("Starting Articy JSon file => %s <= import"),*Filename);
	return true; // Super::FactoryCanImport(Filename);
}

/*
bool UADJsonFactory::HandleImportDuringPlay(UObject* Obj)
{
	const bool bIsPlaying = ArticyImporterHelpers::IsPlayInEditor();
	FArticyEditorModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>(
		"ArticyEditor");

	// if we are already queued, that means we just ended play mode. bIsPlaying will still be true in this case, so we need another check
	if (bIsPlaying && !ArticyImporterModule.IsImportQueued())
	{
		// we have to abuse the module to queue the import since this factory might not exist later on
		ArticyImporterModule.QueueImport();
		return true;
	}

	return false;
}
*/

UObject* UADJsonFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                           const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	//------------------------------------------------------
	// Push outside from the factory if it's not directely
	// related to it (architecture)
	//------------------------------------------------------
#pragma	NOTE( TODO - UADJsonFactory::FactoryCreateFile : analyse updateConfigFile )	
	FString Path = FPaths::GetPath(InParent->GetPathName());
	UpdateConfigFile(Path);
	//------------------------------------------------------
	
	auto ImportData = NewObject<UArticyImportData>(InParent,InName,Flags);
#pragma	NOTE( TODO - UADJsonFactory::FactoryCreateFile )
	// const bool bImportQueued = HandleImportDuringPlay(ArticyImportData);
	
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(
		this, InClass, InParent, InName, *FPaths::GetExtension(Filename));

#pragma	NOTE( TODO - UADJsonFactory::FactoryCreateFile )
	// ImportData->ImportData->Update(GetCurrentFilename());
	
#pragma	NOTE( TODO - UADJsonFactory::FactoryCreateFile ) 
	// if(!bImportQueued &&) { [...] }
	if(!ImportFromFile(Filename, *ImportData) && ImportData)
	{
		ImportData = nullptr;
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, ImportData);
	
	return ImportData;
}


void UADJsonFactory::UpdateConfigFile(FString Path)
{
	UArticyPluginSettings* CDO_Settings = GetMutableDefault<UArticyPluginSettings>();
	if (!CDO_Settings->ArticyDirectory.Path.Equals(Path))
	{
		CDO_Settings->ArticyDirectory.Path = Path;
		FString ConfigName = CDO_Settings->GetDefaultConfigFilename();
		GConfig->SetString(
			TEXT("/Script/ArticyRuntime.ArticyPluginSettings"), TEXT("ArticyDirectory"), *Path, ConfigName);
		GConfig->FindConfigFile(ConfigName)->Dirty = true;
		GConfig->Flush(false, ConfigName);
	}
}

bool UADJsonFactory::ImportFromFile(const FString& FileName, UArticyImportData &Asset) const
{
	//load file as text file
	FString JSON;
	if (!FFileHelper::LoadFileToString(JSON, *FileName))
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}

	// No specific reader needed but the JSon reader itself,
	// we can directly dispatch the file inside the target
	// asset for serialization.
	// Later this logic can be reused for archive multiple files
	// import, hoping to reduce side effects.
	TSharedPtr<FJsonFileDispatcher> Dispatcher = MakeShared<FJsonFileDispatcher>(&Asset);
	if(Dispatcher->HandleFile(JSON))
	{
		// Todo : architecture / strategy
		// to externalyse following logic, as this stage will be the same
		// than previous versions (single or multi JSon files)		
		if(UArticyPluginSettings::Get()->bVerifyArticyReferenceBeforeImport)
		{
			// todo : ArticyRuntime reference check => merge BuildToolParser			
		}

		if(Dispatcher->isCodeGenerationNeeded())
		{
			// Todo : merge code generator
			
			// Todo : proxy (?)... for cache handling (?) 
			// Todo : merge asset generator
			// todo : PostImport method
		}
		else
		{
			// Todo : proxy (?)... for cache handling (?) 
			// Todo : merge asset generator
			// todo : PostImport method
		}
	}
	else return false;		
}

// Reimport handler
bool UADJsonFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
#pragma	NOTE( UADJsonFactory::CanReimport ) 
	return false;
}

void UADJsonFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
#pragma	NOTE( UADJsonFactory::SetReimportPaths )
}

EReimportResult::Type UADJsonFactory::Reimport(UObject* Obj)
{
#pragma	NOTE( UADJsonFactory::Reimport )
	return EReimportResult::Failed;
}