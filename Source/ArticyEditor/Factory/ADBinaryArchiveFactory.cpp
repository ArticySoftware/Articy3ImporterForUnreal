// Fill out your copyright notice in the Description page of Project Settings.


#include "ADBinaryArchiveFactory.h"

#include "ADAssetsBuilders/ADBinaryFileBuilder.h"
#include "ADFileData/AD_FileData.h"
#include "ArticyEditor/ArticyEditor.h"
#include "Misc/FileHelper.h"

UADBinaryArchiveFactory::UADBinaryArchiveFactory()
{
	bEditorImport = true;
	SupportedClass = UAD_FileData::StaticClass();
	Formats.Add(TEXT("archive;Articy:Draft exported file"));
}

bool UADBinaryArchiveFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(ArticyEditor, Log, TEXT("Importation start with file : %s"),*Filename);
	return Super::FactoryCanImport(Filename);
}

UObject* UADBinaryArchiveFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
	bool& bOutOperationCanceled)
{
	FString Path = FPaths::GetPath(InParent->GetPathName());
	
	auto ImportData = NewObject<UAD_FileData>(InParent,InName,Flags);
	
	if(!ImportFromFile(Filename, *ImportData) && ImportData)
	{
		ImportData = nullptr;
	}
	
	return ImportData;
}

bool UADBinaryArchiveFactory::ImportFromFile(const FString& FileName, UAD_FileData& Asset) const
{
	TArray<uint8> fileContent;
	if (!FFileHelper::LoadFileToArray(fileContent, *FileName))
	{
		UE_LOG(ArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}	

	if(fileContent.Num()<=0) return false;

	// FMemoryReader FromBinary = FMemoryReader(fileContent, true);
	// FromBinary.Seek(0);

	ADBinaryFileBuilder::BuildAsset(Asset, fileContent);
	
	//Empty the buffer's contents
	//FromBinary.FlushCache();
	fileContent.Empty();
	//Close the stream
	//FromBinary.Close();
	
	return true;
}