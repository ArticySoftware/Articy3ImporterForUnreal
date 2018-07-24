// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ArticyImportData.h"

/**
 * 
 */
class ARTICYIMPORTER_API FArticyImporterFunctionLibrary
{

public:
	static void ForceCompleteReimport(UArticyImportData* = nullptr);
	static void ReimportChanges(UArticyImportData* = nullptr);
	static void RegenerateAssets(UArticyImportData* = nullptr);
	static bool EnsureImportFile(UArticyImportData**);
};
