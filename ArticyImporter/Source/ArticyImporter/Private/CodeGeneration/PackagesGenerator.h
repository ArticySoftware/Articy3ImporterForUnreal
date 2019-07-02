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

	static bool DoesEditorNeedRestart();

private:
	PackagesGenerator() {}
	virtual ~PackagesGenerator() {}

	static bool LoadAssetsIfNeeded(const TArray<FString>& ObjectPaths, TArray<UObject*>& LoadedObjects, bool bAllowedToPromptToLoadAssets, bool bLoadRedirects);

	static void ExecuteFixUpRedirectorsInGeneratedFolder();

	static UClass* LoadClassFromPath(const FString& Path);


private:
	// we need to restart the engine if an asset was moved via change of package definitions and references were consolidated
	// see UE-29768 for further details
	static bool bEditorNeedsRestart;
};
