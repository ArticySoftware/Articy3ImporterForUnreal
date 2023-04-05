//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.   
//

#pragma once

#include "CoreMinimal.h"
#include "Styling/ISlateStyle.h"

/**  */
class ARTICYEDITOR_API FArticyEditorStyle
{
public:

	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:
	static TSharedRef< class FSlateStyleSet > Create();

private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};