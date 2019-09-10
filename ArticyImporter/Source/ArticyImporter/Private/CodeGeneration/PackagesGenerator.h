//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//
#pragma once
#include "CoreMinimal.h"
#include "ArticyObject.h"
#include "UnrealString.h"

struct ArticyCleanupData;

/**
 * Generates the package assets.
 */

class PackagesGenerator
{
public:

	static void GenerateAssets(class UArticyImportData* Data);

	static const TMap<FString, UArticyObject*> GetCachedExistingObjects();

	static void CacheExistingArticyData(UArticyImportData * Data);

	// cleanup function to be performed after generation has finished
	static void ExecuteCleanup();

	static ArticyCleanupData PostGenerationCleanupData;

private:
	PackagesGenerator() {}
	virtual ~PackagesGenerator() {}

	static void ClearCleanupData();

	/* Compared package names of the cached existing objects and the new imported data
	 * Returns a list of package names currently in the Unreal project that are not part of the articy export anymore */
	static void CacheOutdatedPackageNamesToDelete(UArticyImportData* Data);

private:
	static TMap<FString, UArticyObject*> CachedArticyObjectsMapping;

};
