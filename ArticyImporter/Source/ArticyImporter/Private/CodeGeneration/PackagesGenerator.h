//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once
#include "CoreMinimal.h"
#include "ArticyObject.h"
#include "UnrealString.h"


/**
 * Generates the package assets.
 */
class PackagesGenerator
{
public:

	static void GenerateAssets(class UArticyImportData* Data);

	static const TMap<FString, UArticyObject*> GetCachedExistingObjects();

	static bool DoesEditorNeedRestart();

private:
	PackagesGenerator() {}
	virtual ~PackagesGenerator() {}

	static bool CacheExistingArticyObjects();

	static void ClearExistingObjectsCache();

	static UArticyObject* FindExistingAsset(FArticyId id);

	static bool LoadAssetsIfNeeded(const TArray<FString>& ObjectPaths, TArray<UObject*>& LoadedObjects, bool bAllowedToPromptToLoadAssets, bool bLoadRedirects);

	static void ExecuteFixUpRedirectorsInGeneratedFolder();

	static UClass* LoadClassFromPath(const FString& Path);

	/* Compared package names of the cached existing objects and the new importd data
	 * Returns a list of package names currently in the Unreal project that are not part of the articy export anymore */
	static TArray<FString> GetOutdatedPackageNamesFromCachedObjects(UArticyImportData* Data);


private:
	// we need to restart the engine if an asset was moved via change of package definitions and references were consolidated
	// see UE-29768 for further details
	static bool bEditorNeedsRestart;

	static TMap<FString, UArticyObject*> CachedArticyObjectsMapping;
};
