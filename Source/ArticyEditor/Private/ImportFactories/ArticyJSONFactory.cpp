//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ArticyJSONFactory.h"
#include "CoreMinimal.h"
#include "ArticyImportData.h"
#include "Editor.h"
#include "ArticyEditorModule.h"
#include "ArticyImporterHelpers.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "EditorFramework/AssetImportData.h"
#include "Misc/ConfigCacheIni.h"

#define LOCTEXT_NAMESPACE "ArticyJSONFactory"

UArticyJSONFactory::UArticyJSONFactory()
{
	bEditorImport = true;
	Formats.Add(TEXT("articyue4;A json file exported from articy:draft"));
}

UArticyJSONFactory::~UArticyJSONFactory()
{
}

// @todo : Return type => Poco + deport functonalities inside a builder  
UClass* UArticyJSONFactory::ResolveSupportedClass()
{
	return UArticyImportData::StaticClass();
}

bool UArticyJSONFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(LogArticyEditor, Log, TEXT("Starting Articy JSon file => %s <= import"), *Filename);
	return true;
}

bool UArticyJSONFactory::HandleImportDuringPlay(UObject* Obj)
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

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/**********************************************************************
	 UArticyImportData / export type dependant 
**********************************************************************/


/* @todo :
 *			* ArticyDirectory.path section => wazaitiz? Remove ?
 *			* Precompil/UE4 backward compatibility => Remove
 *			
*			* [!] - Poco type must retain a UArticyImportData to be able to reimport
*					ArticyImportData->ImportData->Update(GetCurrentFilename());
*				
 *			* Deport POCO import functionality inside a builder class (DI/IC)
 *			*		=> !! Return type == Poco !!
*/
UObject* UArticyJSONFactory::FactoryCreateFile(UClass* InClass,
                                               UObject* InParent,
                                               FName InName,
                                               EObjectFlags Flags,
                                               const FString& Filename,
                                               const TCHAR* Parms,
                                               FFeedbackContext* Warn,
                                               bool& bOutOperationCanceled)
{
	// --------- @todo ; analyse (cf method comment) 
	FString Path = FPaths::GetPath(InParent->GetPathName());
	UpdateConfigFile(Path);
	// ---------

	auto ArticyImportData = NewObject<UArticyImportData>(InParent, InName, Flags);
	const bool bImportQueued = HandleImportDuringPlay(ArticyImportData);

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(
		this, InClass, InParent, InName, *FPaths::GetExtension(Filename));

	ArticyImportData->ImportData->Update(GetCurrentFilename());

	if (!bImportQueued)
	{
		if (!ImportFromFile(Filename, ArticyImportData) && ArticyImportData)
		{
			bOutOperationCanceled = true;
			//the asset will be GCed because there are no references to it, no need to delete it
			ArticyImportData = nullptr;
		}
		// Else import should be ok 
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, ArticyImportData);

	return ArticyImportData;
}


/*
 * 	Original comment : properly update the config file and delete previous import assets
 *
 * 	@todo - Relative to (Ticket#2022051610000026) / "Additional Asset Directories to Cook"
 * 	 	   Also, this one is responsible on cooking fail problem due to an  unsolved
 * 	 	   directory path...Looks like an attempt to automatize additional
 * 	 	   Directories to cook ...
 */
void UArticyJSONFactory::UpdateConfigFile(FString Path)
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




/*
 * @todo :
 *			* Main entry process
 *				=> FactoryCreateFile
 *				=> Reimport
 *				@todo : deport inside a builder (DI/IC)
*/
bool UArticyJSONFactory::ImportFromFile(const FString& FileName, UArticyImportData* Asset) const
{
	//load file as text file
	FString JSON;
	if (!FFileHelper::LoadFileToString(JSON, *FileName))
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}

	//parse outermost JSON object
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JSON);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		Asset->MainImportFromJson(JsonParsed);
	}

	return true;
}


/*
 * @todo :
 *			* UArticyImportData to Poco (instead of poco + methods +...)
*			* [!] - Poco type must retain a UArticyImportData to be able to reimport
 *					Asset->ImportData->ExtractFilenames(OutFilenames);
 *			
*/
bool UArticyJSONFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	const auto Asset = Cast<UArticyImportData>(Obj);

	if (!Asset)
	{
		return false;
	}

	const bool bImportQueued = HandleImportDuringPlay(Obj);
	if (bImportQueued)
	{
		return false;
	}

	Asset->ImportData->ExtractFilenames(OutFilenames);
	return true;
}


/*
 *	@todo :
*			* UArticyImportData to Poco (instead of poco + methods +...)
*			* [!] - Poco type must retain a UArticyImportData to be able to reimport
 *				Asset->ImportData->UpdateFilenameOnly(NewReimportPaths[0]);
*/
void UArticyJSONFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	auto Asset = Cast<UArticyImportData>(Obj);
	if (Asset)
		Asset->ImportData->UpdateFilenameOnly(NewReimportPaths[0]);
}


/*
 *	@todo :
*			* UArticyImportData to Poco (instead of poco + methods +...)
*			* [!] - Poco type must retain a UArticyImportData to be able to reimport
 *				Asset->ImportData->...
*/
EReimportResult::Type UArticyJSONFactory::Reimport(UObject* Obj)
{
	auto Asset = Cast<UArticyImportData>(Obj);
	if (Asset)
	{
		if (!Asset->ImportData || Asset->ImportData->GetFirstFilename().Len() == 0)
			return EReimportResult::Failed;

		if (ImportFromFile(Asset->ImportData->GetFirstFilename(), Asset))
			return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}

#undef LOCTEXT_NAMESPACE
