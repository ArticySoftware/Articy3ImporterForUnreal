//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "ArticyEditorModule.h"
#include "ADJsonFactory.generated.h"

/**
 * An Unreal factory that loads generated AD file (Json) in C++ POCO classes.
 */
UCLASS()
class ARTICYEDITOR_API UADJsonFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

	UADJsonFactory();
	
public:
	bool FactoryCanImport(const FString& Filename) override;
	UClass* ResolveSupportedClass() override;
	UObject* FactoryCreateFile(UClass* InClass,
								UObject* InParent, 
								FName InName, 
								EObjectFlags Flags,
								const FString& Filename,
								const TCHAR* Parms,
								FFeedbackContext* Warn,
								bool& bOutOperationCanceled) override;

	bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	EReimportResult::Type Reimport(UObject* Obj) override;
	
private:
#pragma	NOTE( TODO - UADJsonFactory  ->.h )
	void UpdateConfigFile(FString Path);  // What is it for ?
	
	/** Performs the actual import task, converting the json data into UArticyImportData. */	
	bool ImportFromFile(const FString& FileName, class UArticyImportData &Asset) const;

#pragma	NOTE( TODO - UADJsonFactory  ->.h )
	/** If an import or reimport is performed during play, the process is interrupted; upon exiting play a factory is created which then triggers the reimport 
	 * a reimport even in case of the first import works since the ImportDataAsset gets created even during play, which then just needs a reimport to get initialized */
	// bool HandleImportDuringPlay(UObject* Obj);
};
