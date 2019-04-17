//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "ArticyJSONFactory.h"
#include "ArticyImporter.h"
#include "ArticyImportData.h"
#include "Editor.h"

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

	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, *FPaths::GetExtension(Filename));

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

	FEditorDelegates::OnAssetPostImport.Broadcast(this, asset);
	return asset;
}

bool UArticyJSONFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	auto asset = Cast<UArticyImportData>(Obj);
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