//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyJSONFactory.h"
#include "ArticyImporter.h"
#include "ArticyImportData.h"
#include "Editor.h"
#include "ArticyImporterHelpers.h"
#include "CoreMinimal.h"
#include "Runtime/Launch/Resources/Version.h"

#define LOCTEXT_NAMESPACE "ArticyJSONFactory"

UArticyJSONFactory::UArticyJSONFactory()
{
	bEditorImport = true;
	Formats.Add(TEXT("articyue4;A json file exported from articy:draft"));
}

UArticyJSONFactory::~UArticyJSONFactory()
{
}

bool UArticyJSONFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(LogArticyImporter, Log, TEXT("Gonna import %s"), *Filename);

	return true;
}

UClass* UArticyJSONFactory::ResolveSupportedClass()
{
	return UArticyImportData::StaticClass();
}

UObject* UArticyJSONFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	auto ArticyImportData = NewObject<UArticyImportData>(InParent, InName, Flags);

	const bool bImportQueued = HandleImportDuringPlay(ArticyImportData);

#if ENGINE_MINOR_VERSION >= 22
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, *FPaths::GetExtension(Filename));
#else
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, *FPaths::GetExtension(Filename));
#endif

	ArticyImportData->ImportData->Update(GetCurrentFilename());

	if(!bImportQueued)
	{
		if (ImportFromFile(Filename, ArticyImportData) && ArticyImportData)
		{

		}
		else
		{
			bOutOperationCanceled = true;
			//the asset will be GCed because there are no references to it, no need to delete it
			ArticyImportData = nullptr;
		}
	}
	

#if ENGINE_MINOR_VERSION >= 22
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, ArticyImportData);
#else
	FEditorDelegates::OnAssetPostImport.Broadcast(this, ArticyImportData);
#endif

	return ArticyImportData;
}

bool UArticyJSONFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	const auto Asset = Cast<UArticyImportData>(Obj);
	
	const bool bImportQueued = HandleImportDuringPlay(Obj);
	if(bImportQueued)
	{
		return false;
	}

	if(Asset)
	{
		Asset->ImportData->ExtractFilenames(OutFilenames);
		return true;
	}

	return false;
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
		if(!Asset->ImportData || Asset->ImportData->GetFirstFilename().Len() == 0)
			return EReimportResult::Failed;

		if(ImportFromFile(Asset->ImportData->GetFirstFilename(), Asset))
			return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}

bool UArticyJSONFactory::ImportFromFile(const FString& FileName, UArticyImportData* Asset) const
{
	//load file as text file
	FString JSON;
	if (!FFileHelper::LoadFileToString(JSON, *FileName))
	{
		UE_LOG(LogArticyImporter, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}

	//parse outermost JSON object
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JSON);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		Asset->ImportFromJson(JsonParsed);
	}

	return true;
}

bool UArticyJSONFactory::HandleImportDuringPlay(UObject* Obj)
{
	const bool bIsPlaying = ArticyImporterHelpers::IsPlayInEditor();
	FArticyImporterModule& ArticyImporterModule = FModuleManager::Get().GetModuleChecked<FArticyImporterModule>("ArticyImporter");

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