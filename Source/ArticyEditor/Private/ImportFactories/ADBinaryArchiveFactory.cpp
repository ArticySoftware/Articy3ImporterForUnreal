//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//


#include "ADBinaryArchiveFactory.h"

#include "ADAssetsBuilders/ADBinaryFileReader.h"
#include "ADAssetsBuilders/FileDispatcher/JsonFileDispatcher.h"
#include "ADFileData/AD_FileData.h"
#include "ArticyEditor/Public/ArticyEditorModule.h"
#include "Misc/FileHelper.h"

UADBinaryArchiveFactory::UADBinaryArchiveFactory()
{
	bEditorImport = true;
	SupportedClass = UAD_FileData::StaticClass();
	Formats.Add(TEXT("archive;Articy:Draft exported file"));
}

bool UADBinaryArchiveFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(LogArticyEditor, Log, TEXT("Importation start with file : %s"),*Filename);
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
		UE_LOG(LogArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}	

	if(fileContent.Num()<=0) return false;

	//@todo feed :
	//			in IJSonFileDispatcher,
	//			in nullable (optional) logger/perf analyser ,
	//			Out UAD_FileData&
	// -----------------------------------------------------------------------
	// IC / Testable objects section
	// -----------------------------------------------------------------------
	const TUniquePtr<FJsonFileDispatcher> Dispatcher = MakeUnique<FJsonFileDispatcher>();
	const TUniquePtr<ADBinaryFileReader> BinFileReader = MakeUnique<ADBinaryFileReader>(Dispatcher.Get());
	BinFileReader->ReadFile(fileContent);
	
	fileContent.Empty();
	// -----------------------------------------------------------------------
	return true;
}