//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "Misc/FileHelper.h"

class BuildToolParser
{
public:
	
	BuildToolParser(const FString& filePath);
	bool VerifyArticyRuntimeRef();  
	void AddArticyRuntimmeRef(); 
	~BuildToolParser() {};

	static bool AddArticyRuntimeRef(FString& filepath);
	
private:
	// Enforce using parameterized constructor
	BuildToolParser() {};
	int PublicDependencyIndex;
	FString Path = TEXT("");
	TArray <FString> PublicDependencyModulesNames;
	TArray <FString> FileCodeLines;
};