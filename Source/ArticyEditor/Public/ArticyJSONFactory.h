//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//
#pragma once

#include "Factories/Factory.h"
#include "EditorReimportHandler.h"

#include "ArticyJSONFactory.generated.h"

class UArticyImportData;
/**
 * 
 */
UCLASS()
class UArticyJSONFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

	UArticyJSONFactory();
	virtual ~UArticyJSONFactory();

public:
	bool FactoryCanImport(const FString& Filename) override;
	UClass* ResolveSupportedClass() override;
	UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	//FReimportHandler
	// gets called upon pressing "Import"
	bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	EReimportResult::Type Reimport(UObject* Obj) override;
	//~FReimportHandler

private:
	/** Performs the actual import task, converting the json data into UArticyImportData. */
	bool ImportFromFile(const FString& FileName, UArticyImportData* Asset) const;

	/** If an import or reimport is performed during play, the process is interrupted; upon exiting play a factory is created which then triggers the reimport 
	 * a reimport even in case of the first import works since the ImportDataAsset gets created even during play, which then just needs a reimport to get initialized */
	bool HandleImportDuringPlay(UObject* Obj);

};
