#pragma once
#include "Dom/JsonObject.h"
#include "ImportFactories/Data/Settings/ADISettings.h"

/**
 * @brief Provide methods for ADISettings manipulations
 */
class ADISettingsProxy
{
	FADISettings _settings;
	bool bObjectDefsOrGVsChanged = false;
	bool bScriptFragmentsChanged = false;
	
public:
	FADISettings fromJson(const TSharedPtr<FJsonObject> JsonSettings);

// Serialization flags
// => Todo (simplify/refacto if possible...) 
// 	bool DidObjectDefsOrGVsChange() const { return bObjectDefsOrGVsChanged; }
// 	bool DidScriptFragmentsChange() const { return bScriptFragmentsChanged; }
//
// 	void SetObjectDefinitionsRebuilt() { bObjectDefsOrGVsChanged = false; }
// 	void SetScriptFragmentsRebuilt() { bScriptFragmentsChanged = false; }
//
// protected:
// 	//unused in the UE plugin
// 	UPROPERTY(VisibleAnywhere, Category="Settings", meta=(DisplayName="set_Localization - unused in UE"))
// 	bool set_Localization = false;
//
// private:
// 	bool bObjectDefsOrGVsChanged = true;
// 	bool bScriptFragmentsChanged = true;
	
};
