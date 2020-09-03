//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
//

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Slate/GV/SArticyGlobalVariablesDebugger.h"
#include "DetailLayoutBuilder.h"

class FArticyGVCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
private:
	FGlobalVariablesSizeData SizeData;
	// ref: SDetailsView
	float ColumnWidth = 0.65f;
private:
	float OnGetLeftColumnWidth() const { return 1.0f - ColumnWidth; }
	float OnGetRightColumnWidth() const { return ColumnWidth; }
	void OnSetColumnWidth(float InWidth) { ColumnWidth = InWidth; }
};
