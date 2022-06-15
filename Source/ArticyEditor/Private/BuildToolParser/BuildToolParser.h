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

private:
	// Enforce using parameterized constructor
	BuildToolParser() {};
	UINT32 PublicDependencyIndex;
	FString Path = TEXT("");
	TArray <FString> PublicDependencyModulesNames;
	TArray <FString> FileCodeLines;
};