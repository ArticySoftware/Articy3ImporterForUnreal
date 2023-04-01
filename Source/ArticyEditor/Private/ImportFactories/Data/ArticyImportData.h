//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "ArticyGVInfo.h"
#include "Hierarchy/ADIHierarchy.h"
#include "ArticyObjectDefinitions.h"
#include "PackageDefs/ArticyPackageDefs.h"
#include "Project/ArticyProjectDef.h"
#include "Settings/ADISettings.h"
#include "UserMethods/AIDUserMethods.h"
#include "ArticyImportData.generated.h"

/*
    UADFileData
    -----------
    POCO root of an object hierarchy reflecting the raw imported
    AD data hierarchy destined to be serialized as a .uasset.   
 	
 	These hierarchical data are filled from the imported file at
 	the UADJsonFactory (prev. ArticyX) now UADBinaryArchiveFactory.
 */
UCLASS()
class ARTICYEDITOR_API UArticyImportData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category="Hash")
	FString ObjectDefinitionsHash = "";

	UPROPERTY(VisibleAnywhere, Category = "Hash")
	FString ScriptFragmentsHash = "";

	// Todo : add comparison flags for import
	//			Need proxy ?...
	UPROPERTY(VisibleAnywhere, Category="Data")
	FADISettings Settings;

	UPROPERTY(VisibleAnywhere, Category="Data")
	FArticyProjectDef Project;

	UPROPERTY(VisibleAnywhere, Category="Data")
	FArticyGVInfo GlobalVariables;

	// unclomplete class.... not a poco
	// Need a proxy class to implement
	// GetTypes()/GetFeatures() ... etc...
	UPROPERTY(VisibleAnywhere, Category="Data")
	FArticyObjectDefinitions ObjectDefinitions;

	// Add proxy class
	// (GatherScripts()/GenerateAssets()/GetPackageNames()...)
	UPROPERTY(VisibleAnywhere, Category="Data")
	FArticyPackageDefs PackageDefs;

	UPROPERTY(VisibleAnywhere, Category="Scripts")
	FAIDUserMethods UserMethods;

	// Todo : build hierarchy 
	UPROPERTY(VisibleAnywhere, Category="Data")
	FADIHierarchy Hierarchy;

	// Todo : build script fragments
	UPROPERTY(VisibleAnywhere, Category="Scripts")
	TSet<FArticyExpressoFragment> ScriptFragments;

	/*
	// Blueprint type UDataAsset => move elsewhere ??
	// non-poco classes => ADFileAssets ... ? 
	UPROPERTY(VisibleAnywhere, Category = "Assets")
    TArray<TSoftObjectPtr<UArticyPackage>> ImportedPackages;
	*/

	// Optim (ArticyID Hierarchy flattening) => move somewhere else
    // non-poco classes => ADFileCaches
    UPROPERTY(VisibleAnywhere, Category="Cache")
    TMap<FArticyId, FArticyIdArray> ParentChildrenCache;

};
