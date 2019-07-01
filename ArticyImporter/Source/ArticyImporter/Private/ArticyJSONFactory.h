//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
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
	bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	EReimportResult::Type Reimport(UObject* Obj) override;
	//~FReimportHandler

private:

	/** Performs the actual import task, converting the json data into UArticyImportData. */
	bool ImportFromFile(const FString& FileName, UArticyImportData* Asset) const;

	void DelayedReimport(bool bIsSimulating, UObject* obj);
	void ActivateDelayedReimport(UObject* Obj);

};
