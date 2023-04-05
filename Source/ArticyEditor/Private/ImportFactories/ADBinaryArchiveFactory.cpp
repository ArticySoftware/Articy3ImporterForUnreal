//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//


#include "ADBinaryArchiveFactory.h"

#include "ADAssetsBuilders/ADBinaryFileReader.h"
#include "ADAssetsBuilders/FileDispatcher/BinaryFileDispatcher.h"
#include "ImportFactories/ArticyImportData.h"
#include "ArticyEditor/Public/ArticyEditorModule.h"
#include "Misc/FileHelper.h"

UADBinaryArchiveFactory::UADBinaryArchiveFactory()
{
	bEditorImport = true;
	SupportedClass = UArticyImportData::StaticClass();
	Formats.Add(TEXT("archive;Articy:Draft exported file"));
}

UClass* UADBinaryArchiveFactory::ResolveSupportedClass()
{
	return UArticyImportData::StaticClass();
}

bool UADBinaryArchiveFactory::FactoryCanImport(const FString& Filename)
{
	UE_LOG(LogArticyEditor, Log, TEXT("Importation start with file : %s"),*Filename);
	return true;
}

UObject* UADBinaryArchiveFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
                                                    EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
                                                    bool& bOutOperationCanceled)
{
	FString Path = FPaths::GetPath(InParent->GetPathName());
	
	auto ImportData = NewObject<UArticyImportData>(InParent,InName,Flags);
	
	if(!ImportFromFile(Filename, *ImportData) && ImportData)
	{
		ImportData = nullptr;
	}
	
	return ImportData;
}

bool UADBinaryArchiveFactory::ImportFromFile(const FString& FileName, UArticyImportData& Asset) const
{
	TArray<uint8> fileContent;
	if (!FFileHelper::LoadFileToArray(fileContent, *FileName))
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *FileName);
		return false;
	}	

	if(fileContent.Num()<=0) return false;

	//@todo feed :
	//			- in VisitorStack that operates over the importData structure
	//				to handle post-dispatcher operations
	//			- in nullable (optional) logger/perf analyser
	//			- Data proxyes that encapsulate imported data structures
	//				and provides methods to manipulate imported/serialized data
	// -----------------------------------------------------------------------
	// IC / Testable objects section
	// -----------------------------------------------------------------------
	const TSharedPtr<FBinaryFileDispatcher> Dispatcher = MakeShared<FBinaryFileDispatcher>(&Asset);
	const TSharedPtr<ADBinaryFileReader> BinFileReader = MakeShared<ADBinaryFileReader>(Dispatcher);
	BinFileReader->ReadFile(fileContent);
	
	fileContent.Empty();
	// -----------------------------------------------------------------------
	return true;
}