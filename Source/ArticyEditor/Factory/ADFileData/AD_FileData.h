//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ADGV.h"
#include "ADHierarchy.h"
#include "ADObjectDefs.h"
#include "ADPackages.h"
#include "ADProjectDef.h"
#include "ADSettings.h"
#include "ADUserMethods.h"
#include "AD_FileData.generated.h"

//struct FADExpressoFragment;
/**
 * UADFileData
 * -----------
 *	This is the root class of a plain old class object (POCO) data hierarchy
 *	representation of the Articy:Draft exported file, serialized inside a
 *	Unreal uasset.
 *	
 *	These hierarchical data are filled from the imported file at
 *	the UADJsonFactory.
 */
UCLASS()
class ARTICYEDITOR_API UAD_FileData : public UDataAsset
{
	GENERATED_BODY()

    

public:
	UPROPERTY(VisibleAnywhere, Category="Data")
	FADSettings Settings;

	UPROPERTY(VisibleAnywhere, Category="Data")
	FADProjectDef ProjectDefinitions;

	UPROPERTY(VisibleAnywhere, Category="Data")
	FADGV GlobalVariables;

	UPROPERTY(VisibleAnywhere, Category="Data")
	FADObjectDefs ObjectDefinitions;

	UPROPERTY(VisibleAnywhere, Category="Data")
	FADPackages Packages;

	UPROPERTY(VisibleAnywhere, Category="Data")
	FADHierarchy Hierarchy;

	UPROPERTY(VisibleAnywhere, Category="Scripts")
	FADUserMethods UserMethods;

	// UPROPERTY(VisibleAnywhere, Category="Scripts")
	// TSet<FADExpressoFragment> ScriptFragments;
	
	/*
	// Blueprint type UDataAsset => move elsewhere ??
	// non-poco classes => ADFileAssets ... ? 
	UPROPERTY(VisibleAnywhere, Category = "Assets")
    TArray<TSoftObjectPtr<UArticyPackage>> ImportedPackages;

    // Optim (ArticyID Hierarchy flattening) => move somewhere else
    // non-poco classes => ADFileCaches
    UPROPERTY(VisibleAnywhere, Category="Cache")
    TMap<FArticyId, FArticyIdArray> ParentChildrenCache;
	*/
};
