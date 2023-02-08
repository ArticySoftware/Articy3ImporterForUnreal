//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#include "BuildToolParser.h"
#include "ArticyEditorModule.h"

BuildToolParser::BuildToolParser(const FString& filePath)
{
	Path = filePath;
}

bool BuildToolParser::VerifyArticyRuntimeRef()
{
	// Open the Path file and read its content
	// Then parse the file to find a "ArticyRuntime" Reference inside the public dependency array
	if (!FFileHelper::LoadANSITextFileToStrings(*Path,NULL ,FileCodeLines ))
	{
		UE_LOG(LogArticyEditor, Error, TEXT("Failed to load file '%s' to string"), *Path);
		return false;
	}

	PublicDependencyIndex = 0;
	for (auto &Str : FileCodeLines)
	{
		if (Str.Contains(TEXT("PublicDependencyModuleNames.AddRange"), ESearchCase::CaseSensitive))
		{
			Str.RemoveSpacesInline();
			Str.ReplaceInline(TEXT("\t"), TEXT(""));
			Str.ReplaceInline(TEXT("PublicDependencyModuleNames.AddRange(newstring[]{"),TEXT(""));
			Str.ReplaceInline(TEXT( "});"),TEXT(""));
			Str.ParseIntoArray(PublicDependencyModulesNames, TEXT(","));

			if (PublicDependencyModulesNames.Find(TEXT("\"ArticyRuntime\"")) != INDEX_NONE)
				return true;
			else
				return false;
		}
		PublicDependencyIndex++;
	}

	return false;
}

void BuildToolParser::AddArticyRuntimmeRef()
{
	PublicDependencyModulesNames.Emplace(TEXT("\"ArticyRuntime\""));
	
	FString newDepArray = "\t\tPublicDependencyModuleNames.AddRange(new string[] {";
	for (int32 Index = 0; Index != PublicDependencyModulesNames.Num(); ++Index)
	{
		if(Index < PublicDependencyModulesNames.Num()-1)
			newDepArray += PublicDependencyModulesNames[Index] + ",";
		else
			newDepArray += PublicDependencyModulesNames[Index];
	}
	newDepArray += "});";

	FileCodeLines[PublicDependencyIndex] = newDepArray;

	FFileHelper::SaveStringArrayToFile(FileCodeLines, *Path);
}
