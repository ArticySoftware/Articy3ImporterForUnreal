//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyJSONFactory.h"
#include "CoreMinimal.h"
#include "ArticyArchiveReader.h"
#include "ArticyImportData.h"
#include "Editor.h"
#include "ArticyEditorModule.h"
#include "ArticyImporterHelpers.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Runtime/Launch/Resources/Version.h"
#include "EditorFramework/AssetImportData.h"
#include "Misc/ConfigCacheIni.h"

#define LOCTEXT_NAMESPACE "ArticyJSONFactory"

UArticyJSONFactory::UArticyJSONFactory()
{
	bEditorImport = true;
	Formats.Add(TEXT("articyue;A json file exported from articy:draft X"));
}

UArticyJSONFactory::~UArticyJSONFactory()
{
}

bool UArticyJSONFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(LogArticyEditor, Log, TEXT("Gonna import %s"), *Filename);

	return true;
}

UClass* UArticyJSONFactory::ResolveSupportedClass()
{
	return UArticyImportData::StaticClass();
}

UObject* UArticyJSONFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString Path = FPaths::GetPath(InParent->GetPathName());

	// properly update the config file and delete previous import assets
	UArticyPluginSettings* CDO_Settings = GetMutableDefault<UArticyPluginSettings>();
    if(!CDO_Settings->ArticyDirectory.Path.Equals(Path))
	{
    	// @Alewinn - Relative to (Ticket#2022051610000026) / "Additional Asset Directories to Cook"
    	//			  Also, this one is responsible on cooking fail problem due to an  unsolved directory path...
    	//			  Looks like an attempt to automatize additional Directories to cook ... 
		CDO_Settings->ArticyDirectory.Path = Path;
		FString ConfigName = CDO_Settings->GetDefaultConfigFilename();
		GConfig->SetString(TEXT("/Script/ArticyRuntime.ArticyPluginSettings"), TEXT("ArticyDirectory"), *Path, ConfigName);
		GConfig->FindConfigFile(ConfigName)->Dirty = true;
		GConfig->Flush(false, ConfigName);
    }
	
	auto ArticyImportData = NewObject<UArticyImportData>(InParent, InName, Flags);

	const bool bImportQueued = HandleImportDuringPlay(ArticyImportData);

#if ENGINE_MAJOR_VERSION >= 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 22)
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, *FPaths::GetExtension(Filename));
#else
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, *FPaths::GetExtension(Filename));
#endif

	ArticyImportData->ImportData->Update(GetCurrentFilename());

	if(!bImportQueued)
	{
		if (!ImportFromFile(Filename, ArticyImportData) && ArticyImportData)
		{
			bOutOperationCanceled = true;
			//the asset will be GCed because there are no references to it, no need to delete it
			ArticyImportData = nullptr;
		}
		// Else import should be ok 
	}
	

#if ENGINE_MAJOR_VERSION >= 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 22)
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, ArticyImportData);
#else
	FEditorDelegates::OnAssetPostImport.Broadcast(this, ArticyImportData);
#endif

	return ArticyImportData;
}

bool UArticyJSONFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	const auto Asset = Cast<UArticyImportData>(Obj);

	if(!Asset)
	{
		return false;
	}

	const bool bImportQueued = HandleImportDuringPlay(Obj);
	if(bImportQueued)
	{
		return false;
	}

	Asset->ImportData->ExtractFilenames(OutFilenames);
	return true;
}

void UArticyJSONFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	auto Asset = Cast<UArticyImportData>(Obj);
	if(Asset)
		Asset->ImportData->UpdateFilenameOnly(NewReimportPaths[0]);
}

EReimportResult::Type UArticyJSONFactory::Reimport(UObject* Obj)
{
	auto Asset = Cast<UArticyImportData>(Obj);
	if(Asset)
	{
		if(!Asset->ImportData)
			return EReimportResult::Failed;

		// Don't look for old .articyue4 files
		if (Asset->ImportData->SourceData.SourceFiles.Num() > 0)
			Asset->ImportData->SourceData.SourceFiles[0].RelativeFilename.RemoveFromEnd(TEXT("4"));
		
		const FString ImportFilename = Asset->ImportData->GetFirstFilename();
		
		if(ImportFilename.Len() == 0)
			return EReimportResult::Failed;

		if(ImportFromFile(ImportFilename, Asset))
			return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}

bool UArticyJSONFactory::ImportFromFile(const FString& FileName, UArticyImportData* Asset) const
{
	UArticyArchiveReader* Archive = NewObject<UArticyArchiveReader>();
	Archive->OpenArchive(*FileName);
	
	//load file as text file
	FString JSON;
	if (!Archive->ReadFile(TEXT("manifest.json"), JSON))
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}

	//parse outermost JSON object
	TSharedPtr<FJsonObject> JsonParsed;
	const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JSON);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		Asset->ImportFromJson(*Archive, JsonParsed);
	}

	return true;
}

bool UArticyJSONFactory::HandleImportDuringPlay(UObject* Obj)
{
	const bool bIsPlaying = ArticyImporterHelpers::IsPlayInEditor();
	FArticyEditorModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyEditorModule>("ArticyEditor");

	// if we are already queued, that means we just ended play mode. bIsPlaying will still be true in this case, so we need another check
	if (bIsPlaying && !ArticyImporterModule.IsImportQueued())
	{
		// we have to abuse the module to queue the import since this factory might not exist later on
		ArticyImporterModule.QueueImport();
		return true;
	}

	return false;
}

#undef LOCTEXT_NAMESPACE