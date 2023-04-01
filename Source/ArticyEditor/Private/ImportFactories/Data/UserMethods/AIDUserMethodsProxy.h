#pragma once
#include "AIDUserMethods.h"

class AIDUserMethodsProxy
{
	FAIDUserMethods _userMethods;
public:
	FAIDUserMethods fromJson(const TArray<TSharedPtr<FJsonValue>>* Json);
	
	// Todo : FAIDScriptMethod proxy  
	// const FString& GetCPPReturnType() const;
	// const FString& GetCPPDefaultReturn() const;
	// const FString GetCPPParameters() const;
	// const FString GetArguments() const;
	// const FString GetOriginalParametersForDisplayName() const;
	
private:
	void GetScriptMethod(FAIDScriptMethod& scriptMethod,TSharedPtr<FJsonObject> Json,TSet<FString>& OverloadedMethods);
};
