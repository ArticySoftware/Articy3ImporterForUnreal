//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "ArticyRuntimeModule.h"
#include "Internationalization/StringTableRegistry.h"

DEFINE_LOG_CATEGORY(LogArticyRuntime)

void FArticyRuntimeModule::StartupModule()
{
	IterateStringTables(FPaths::ProjectContentDir() / "ArticyContent/Generated");
}

void FArticyRuntimeModule::ShutdownModule()
{

}

void FArticyRuntimeModule::IterateStringTables(const FString& DirectoryPath) const
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	if (PlatformFile.DirectoryExists(*DirectoryPath))
	{
		TArray<FString> FoundFiles;
		PlatformFile.FindFiles(FoundFiles, *DirectoryPath, TEXT(".csv"));

		for (const FString& FilePath : FoundFiles)
		{
			FString StringTable = FPaths::GetBaseFilename(*FilePath, true);
			FStringTableRegistry::Get().Internal_LocTableFromFile(
				FName(StringTable),
				StringTable,
				"ArticyContent/Generated/" + StringTable + ".csv",
				FPaths::ProjectContentDir());
		}
	}
}

IMPLEMENT_MODULE(FArticyRuntimeModule, ArticyRuntime)