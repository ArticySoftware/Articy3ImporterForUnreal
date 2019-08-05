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

private:
	PackagesGenerator() {}
	virtual ~PackagesGenerator() {}

	static bool CacheExistingArticyObjects();

	static void ClearExistingObjectsCache();

	/* Compared package names of the cached existing objects and the new imported data
	 * Returns a list of package names currently in the Unreal project that are not part of the articy export anymore */
	static TArray<FString> GetOutdatedPackageNamesFromCachedObjects(UArticyImportData* Data);

private:
	static TMap<FString, UArticyObject*> CachedArticyObjectsMapping;
};
