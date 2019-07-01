//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyJSONFactory.h"
#include "ArticyImporter.h"
#include "ArticyImportData.h"
#include "Editor.h"
#include <Dialogs.h>
#include "ArticyImporterHelpers.h"

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
	auto asset = NewObject<UArticyImportData>(InParent, InName, Flags);

	bool bIsPlaying = ArticyImporterHelpers::IsPlayInEditor();
	FArticyImporterModule& importerModule = FModuleManager::Get().GetModuleChecked<FArticyImporterModule>("ArticyImporter");

	// if we are already queued, that means we just ended play mode. bIsPlaying will still be true in this case, so we need another check
	if(bIsPlaying && !importerModule.IsImportQueued()) 
	{
		// we have to abuse the module to queue the import since this factory will be deleted immediately
		ArticyImporterHelpers::ArticyImportCreationData data(InClass, InParent, InName, Flags, Filename,Parms, Warn, bOutOperationCanceled);
		importerModule.QueueImport(data);
		return nullptr;
	}

#if ENGINE_MINOR_VERSION >= 22
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, *FPaths::GetExtension(Filename));
#else
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, *FPaths::GetExtension(Filename));
#endif

	if(ImportFromFile(Filename, asset) && asset)
	{
		asset->ImportData->Update(GetCurrentFilename());
	}
	else
	{
		bOutOperationCanceled = true;
		//the asset will be GCed because there are no references to it, no need to delete it
		asset = nullptr;
	}

#if ENGINE_MINOR_VERSION >= 22
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, asset);
#else
	FEditorDelegates::OnAssetPostImport.Broadcast(this, asset);
#endif

	return asset;
}

bool UArticyJSONFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{

	// gets called upon pressing "Import"
	auto asset = Cast<UArticyImportData>(Obj);
	bool bIsPlaying = ArticyImporterHelpers::IsPlayInEditor();

	if (bIsPlaying && asset)
	{
		ActivateDelayedReimport(Obj);

		UE_LOG(LogArticyImporter, Log, TEXT("Can't import right now!"));
		return false;
	}

	if(asset)
	{
		asset->ImportData->ExtractFilenames(OutFilenames);
		return true;
	}

	return false;
}

void UArticyJSONFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	auto asset = Cast<UArticyImportData>(Obj);
	if(asset)
		asset->ImportData->UpdateFilenameOnly(NewReimportPaths[0]);
}

EReimportResult::Type UArticyJSONFactory::Reimport(UObject* Obj)
{
	auto asset = Cast<UArticyImportData>(Obj);
	if(asset)
	{
		if(!asset->ImportData || asset->ImportData->GetFirstFilename().Len() == 0)
			return EReimportResult::Failed;

		if(ImportFromFile(asset->ImportData->GetFirstFilename(), asset))
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

void UArticyJSONFactory::DelayedReimport(bool bIsSimulating, UObject* obj)
{
	FEditorDelegates::EndPIE.RemoveAll(this);

	if (obj->IsValidLowLevel())
	{
		Reimport(obj);
	}
}

void UArticyJSONFactory::ActivateDelayedReimport(UObject* Obj)
{
	FEditorDelegates::EndPIE.AddUObject(this, &UArticyJSONFactory::DelayedReimport, Obj);

	FOnMsgDlgResult OnDialogClosed;
	FText Message = LOCTEXT("ImportWhilePlaying", "To import articy:draft data, the play mode has to be exited. Import will begin after exiting play.");
	FText Title = LOCTEXT("ImportWhilePlaying_Title", "Import not possible");
	TSharedRef<SWindow> window = OpenMsgDlgInt_NonModal(EAppMsgType::Ok, Message, Title, OnDialogClosed);
}

#undef LOCTEXT_NAMESPACE