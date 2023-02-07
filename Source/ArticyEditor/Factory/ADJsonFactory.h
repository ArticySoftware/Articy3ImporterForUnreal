//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ADJsonFactory.generated.h"

/**
 * An Unreal factory that loads generated AD file (Json) in C++ POCO classes.
 */
UCLASS()
class ARTICYEDITOR_API UADJsonFactory : public UFactory
{
	GENERATED_BODY()

public:
	bool FactoryCanImport(const FString& Filename) override;
	UObject* FactoryCreateFile(UClass* InClass,
								UObject* InParent, 
								FName InName, 
								EObjectFlags Flags,
								const FString& Filename,
								const TCHAR* Parms,
								FFeedbackContext* Warn,
								bool& bOutOperationCanceled) override;
	
private:
	UADJsonFactory();
	/** Performs the actual import task, converting the json data into UArticyImportData. */
	bool ImportFromFile(const FString& FileName, class UAD_FileData &Asset) const;
	
};
