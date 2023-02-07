// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ADBinaryArchiveFactory.generated.h"

/**
 * 
 */
UCLASS()
class ARTICYEDITOR_API UADBinaryArchiveFactory : public UFactory
{
	GENERATED_BODY()
	
	UADBinaryArchiveFactory();
	bool ImportFromFile(const FString& FileName, class UAD_FileData &Asset) const;
	
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
	
};
