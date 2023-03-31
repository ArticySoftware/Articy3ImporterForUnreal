#include "JsonFileDispatcher.h"

#include "ArticyEditorModule.h"

void FJsonFileDispatcher::HandleManifest(FString JsonManifest)
{
	_asset->Settings.ExportVersion = TEXT("This is a test !!");
	
	UE_LOG(LogArticyEditor, Warning, TEXT("*************************************"));
	UE_LOG(LogArticyEditor, Warning, TEXT("*** Manifest Dispatcher triggered ***"));
	UE_LOG(LogArticyEditor, Warning, TEXT("*************************************"));	
}
