//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

enum class ECheckBoxState : unsigned char;

class ARTICYEDITOR_API SPackageSettings : public SCompoundWidget
{
public:
		SLATE_BEGIN_ARGS(SPackageSettings) {}

		SLATE_ARGUMENT(FName, PackageToDisplay)

		SLATE_END_ARGS()

/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
	void Construct(const FArguments& InArgs);
private:
	FName PackageToDisplay;

	FText GetPackageName() const;

	void OnCheckStateChanged(ECheckBoxState NewState) const;

	ECheckBoxState IsChecked() const;
};
