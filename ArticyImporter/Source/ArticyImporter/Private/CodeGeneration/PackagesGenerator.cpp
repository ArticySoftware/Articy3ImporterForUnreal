//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "PackagesGenerator.h"
#include "ArticyImportData.h"
#include <ObjectTools.h>
#include <AssetRegistryModule.h>
#include <AssetData.h>
#include <ModuleManager.h>
#include <AssetToolsModule.h>

void PackagesGenerator::GenerateAssets(UArticyImportData* Data)
{
	// get all currently existing articy objects
	bool bDoOldFilesExist = false;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> ExistingObjectsData;
	AssetRegistryModule.Get().GetAssetsByClass(UArticyObject::StaticClass()->GetFName(), ExistingObjectsData, true);

	
	if (ExistingObjectsData.Num() > 0) {
		bDoOldFilesExist = true;
	}

	TMap<FString, UArticyObject*> existingObjectsMapping;
	for (FAssetData data : ExistingObjectsData) {
		UArticyObject* obj = Cast<UArticyObject>(data.GetAsset());
		existingObjectsMapping.Add(obj->GetName(), obj);
	}

	TArray<UArticyObject*> OldObjects;
	existingObjectsMapping.GenerateValueArray(OldObjects);

	// mark all previously existing articy objects to be destroyed
	TArray<UObject*> objectsToDelete;
	for (UArticyObject* obj : OldObjects) {

		UObject* uobj = obj;
		objectsToDelete.Add(uobj);
	}

	// generate new articy objects
	auto pack = Data->GetPackageDefs();
	pack.GenerateAssets(Data);

	TArray<FArticyPackage> packages = Data->GetPackages();
	TMap<FString, UArticyObject*> newObjectsMapping;

	for (FArticyPackage package : packages)
	{
		TArray<UArticyPrimitive*> containedObjects = package.Objects;
		for (UArticyPrimitive* prim : containedObjects) {
			UArticyObject* obj = Cast<UArticyObject>(prim);
			if (obj)
			{
				newObjectsMapping.Add(obj->GetName(), obj);
			}
		}
	}

	// if old files exist, delete the ones that don't exist in any package anymore
	// if a file was simply moved by means of packaging rulesets, update the references and delete the old ones
	if (bDoOldFilesExist) 
	{
		for (FArticyPackage package : packages) 
		{
			TArray<UArticyPrimitive*> containedObjects = package.Objects;

			for(UArticyObject * oldObject : OldObjects) 
			{
				bool bDeleteOldFile = true;
				if(!oldObject->IsValidLowLevel()) break;
				if (newObjectsMapping.Contains(oldObject->GetName()))
				{
					UArticyObject* newObject = newObjectsMapping[oldObject->GetName()];

					// if the objects are different despite having the same name/ID
					// that means the newObject was moved to another package
					// Update references and delete oldObject
					if (newObject != oldObject) 
					{
						UObject* newUObject = newObject;
						UObject* oldUObject = oldObject;

						TArray<UObject*> oldUObjects;
						oldUObjects.Add(oldUObject);

						TSet<UObject*> ObjectsToConsolidateWithin;
						TSet<UObject*> ObjectsToNotConsolidateWithin;

						// updates the reference
						ObjectTools::FConsolidationResults results = ObjectTools::ConsolidateObjects(newUObject, oldUObjects, ObjectsToConsolidateWithin, ObjectsToNotConsolidateWithin, false);
					}

					else 
					{
						// if they are the same objects, that means the files just got updated
						bDeleteOldFile = false;
					}

					if (!bDeleteOldFile) 
					{
						objectsToDelete.Remove(oldObject);
					}
				}
			}
		}

		ObjectTools::ForceDeleteObjects(objectsToDelete, false);
	}
}
