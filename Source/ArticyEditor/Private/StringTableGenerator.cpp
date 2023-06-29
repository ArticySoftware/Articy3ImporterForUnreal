//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "StringTableGenerator.h"
#include "Misc/App.h"
#include "Misc/FileHelper.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "HAL/PlatformFilemanager.h"
#include "SourceControlHelpers.h"

void StringTableGenerator::Line(const FString& Key, const FString& SourceString)
{
	FileContent += TEXT("\"") + Key + TEXT(",") + SourceString + TEXT("\n");
}

void StringTableGenerator::WriteToFile() const
{
	if(FileContent.IsEmpty())
		return;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	ISourceControlModule& SCModule = ISourceControlModule::Get();

	bool bCheckOutEnabled = false;
	if(SCModule.IsEnabled())
	{
		bCheckOutEnabled = ISourceControlModule::Get().GetProvider().UsesCheckout();
	}
	
	// try check out the file if it existed
	bool bFileExisted = false;
	if(PlatformFile.FileExists(*Path) && bCheckOutEnabled)
	{
		USourceControlHelpers::CheckOutFile(*Path);
		bFileExisted = true;
	}
	
	const bool bFileWritten = FFileHelper::SaveStringToFile(FileContent, *Path, FFileHelper::EEncodingOptions::ForceUTF8);

	// mark the file for add if it's the first time we've written it
	if(!bFileExisted && bFileWritten && SCModule.IsEnabled())
	{
		USourceControlHelpers::MarkFileForAdd(*Path);
	}
}
