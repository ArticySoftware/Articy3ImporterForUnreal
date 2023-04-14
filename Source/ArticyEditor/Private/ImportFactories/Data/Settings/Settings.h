#pragma once
#include "Settings.generated.h"

/**
 * The Settings object in the .json file.
 */
USTRUCT()
struct FADISettings
{
	GENERATED_BODY()
	
	// --- ArticyX multiJson file + localization
	UPROPERTY(VisibleAnywhere, Category="Settings")
	bool set_Localization = false;

	UPROPERTY(VisibleAnywhere, Category="Settings")
	TArray<FString> set_IncludedNodes;

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString	set_UsedLanguage;
	// ------------------------------------------------
	
	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString set_TextFormatter = "";

	/** If this is false, no ExpressoScripts class is generated,
		and script fragments are not evaluated/executed. */
	UPROPERTY(VisibleAnywhere, Category="Settings")
	bool set_UseScriptSupport = false;

	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ExportVersion = "";
	
	UPROPERTY(VisibleAnywhere, Category="Settings")
	FString ObjectDefinitionsHash = "";

	UPROPERTY(VisibleAnywhere, Category = "Settings")
	FString ScriptFragmentsHash = "";

	void ImportFromJson(const TSharedPtr<FJsonObject> JsonRoot);

	// !!!!!!!!! Alewinn ; for implementation only !!! --------------------------------------
	//	=> This have to be moved into the manifest structure
	bool DidObjectDefsOrGVsChange() const { return true;  } //bObjectDefsOrGVsChanged;
	bool DidScriptFragmentsChange() const { return true; } //bScriptFragmentsChanged

	void SetObjectDefinitionsRebuilt() { bObjectDefsOrGVsChanged = false; }
	void SetScriptFragmentsRebuilt() { bScriptFragmentsChanged = false; }

private:
	bool bObjectDefsOrGVsChanged = true;
	bool bScriptFragmentsChanged = true;
	// !!!!!!!!! /Alewinn  -------------------------------------------------------------------
};
