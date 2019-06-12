//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once

/**
 * Generates the package assets.
 */
class PackagesGenerator
{
public:

	static void GenerateAssets(class UArticyImportData* Data);

private:
	PackagesGenerator() {}
	virtual ~PackagesGenerator() {}

	static bool LoadAssetsIfNeeded(const TArray<FString>& ObjectPaths, TArray<UObject*>& LoadedObjects, bool bAllowedToPromptToLoadAssets, bool bLoadRedirects);

	static void ExecuteFixUpRedirectorsInGeneratedFolder();

	static UClass* LoadClassFromPath(const FString& Path);
};
