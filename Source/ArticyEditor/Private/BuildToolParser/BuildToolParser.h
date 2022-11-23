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
	
	/**
	 * @brief Return new string where single and multi line C# comments have been removed.
	 * @param line : the string from which comments must be removed.
	 * @return a new string where comments have been removed.
	 */
	FString RemoveComments(FString line);

	/**
	 * @brief Return a list of all dependency modules lines from a string
	 * @param line : the string without comments  
	 * @return a list of the dependency modules c# code lines
	 */
	bool CheckReferences(FString line);
	
	int PublicDependencyIndex;
	FString Path = TEXT("");
	TArray <FString> PublicDependencyModulesNames;
	TArray <FString> FileCodeLines;
};