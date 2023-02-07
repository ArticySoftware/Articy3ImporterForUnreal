//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//


#include "ADJsonFactory.h"
#include "ADAssetsBuilders/ADJsonFileBuilder.h"
#include "ADFileData/AD_FileData.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "ArticyEditor/ArticyEditor.h"

UADJsonFactory::UADJsonFactory()
{
	bEditorImport = true;
	SupportedClass = UAD_FileData::StaticClass();
	Formats.Add(TEXT("articyue4;Articy:Draft exported file"));
}

bool UADJsonFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(ArticyEditor, Log, TEXT("Importation start with file : %s"),*Filename);
	return Super::FactoryCanImport(Filename);
}

UObject* UADJsonFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString Path = FPaths::GetPath(InParent->GetPathName());
	
	auto ImportData = NewObject<UAD_FileData>(InParent,InName,Flags);
	
	if(!ImportFromFile(Filename, *ImportData) && ImportData)
	{
		ImportData = nullptr;
	}
	
	return ImportData;
}

bool UADJsonFactory::ImportFromFile(const FString& FileName, UAD_FileData &Asset) const
{
	//load file as text file
	FString JSON;
	if (!FFileHelper::LoadFileToString(JSON, *FileName))
	{
		UE_LOG(ArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}

	//parse outermost JSON object
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JSON);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		ADJsonFileBuilder::BuildAsset(Asset,JsonParsed);
	}

	return true;
}