//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//

#include "ArticyImporterStyle.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FArticyImporterStyle::StyleInstance = NULL;

void FArticyImporterStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FArticyImporterStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FArticyImporterStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ArticyImporterStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon96x96(96.0f, 96.0f);



TSharedRef< FSlateStyleSet > FArticyImporterStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ArticyImporterStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ArticyImporter")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ArticyImporter.OpenPluginWindow", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));
	Style->Set("ArticyImporter.ArticyDraftLogo.16", new IMAGE_BRUSH(TEXT("ArticyDraft16"), Icon16x16));
	Style->Set("ArticyImporter.ArticyDraftLogo.32", new IMAGE_BRUSH(TEXT("ArticyDraft32"), Icon32x32));
	Style->Set("ArticyImporter.ArticySoftwareLogo.16", new IMAGE_BRUSH(TEXT("ArticySoftware16"), Icon16x16));
	Style->Set("ArticyImporter.ArticySoftwareLogo.32", new IMAGE_BRUSH(TEXT("ArticySoftware32"), Icon32x32));
	Style->Set("ArticyImporter.ArticyApplication.64", new IMAGE_BRUSH(TEXT("128/application"), Icon64x64));

	Style->Set("ArticyImporter.AssetPicker.TileBackground.96", new IMAGE_BRUSH(TEXT("128/content_tile_fill"), Icon96x96));
	Style->Set("ArticyImporter.AssetPicker.TileBorder.16", new BOX_BRUSH(TEXT("Border"), 4.f/16.f));

	Style->Set("ArticyImporter.Type.DialogueFragment.16", new IMAGE_BRUSH(TEXT("128/dialoguefragment"), Icon16x16));
	Style->Set("ArticyImporter.Type.Dialogue.16", new IMAGE_BRUSH(TEXT("128/dialogue2"), Icon16x16));
	Style->Set("ArticyImporter.Type.Entity.16", new IMAGE_BRUSH(TEXT("128/entity"), Icon16x16));
	Style->Set("ArticyImporter.Type.FlowFragment.16", new IMAGE_BRUSH(TEXT("128/flowfragment"), Icon16x16));
	Style->Set("ArticyImporter.Type.Hub.16", new IMAGE_BRUSH(TEXT("128/hub"), Icon16x16));
	Style->Set("ArticyImporter.Type.Jump.16", new IMAGE_BRUSH(TEXT("128/jump"), Icon16x16));
	Style->Set("ArticyImporter.Type.Instruction.16", new IMAGE_BRUSH(TEXT("128/instruction"), Icon16x16));
	Style->Set("ArticyImporter.Type.Condition.16", new IMAGE_BRUSH(TEXT("128/condition"), Icon16x16));
	Style->Set("ArticyImporter.Type.Asset.16", new IMAGE_BRUSH(TEXT("128/asset"), Icon16x16));
	Style->Set("ArticyImporter.Type.UserFolder.16", new IMAGE_BRUSH(TEXT("128/assetfolder"), Icon16x16));
	Style->Set("ArticyImporter.Type.Zone.16", new IMAGE_BRUSH(TEXT("128/zone"), Icon16x16));
	Style->Set("ArticyImporter.Type.Location.16", new IMAGE_BRUSH(TEXT("128/location"), Icon16x16));
	Style->Set("ArticyImporter.Type.LocationImage.16", new IMAGE_BRUSH(TEXT("128/locationimage"), Icon16x16));
	Style->Set("ArticyImporter.Type.Link.16", new IMAGE_BRUSH(TEXT("128/link"), Icon16x16));
	Style->Set("ArticyImporter.Type.Comment.16", new IMAGE_BRUSH(TEXT("128/comment"), Icon16x16));
	
	Style->Set("ArticyImporter.Type.DialogueFragment.32", new IMAGE_BRUSH(TEXT("128/dialoguefragment"), Icon32x32));
	Style->Set("ArticyImporter.Type.Dialogue.32", new IMAGE_BRUSH(TEXT("128/dialogue2"), Icon32x32));
	Style->Set("ArticyImporter.Type.Entity.32", new IMAGE_BRUSH(TEXT("128/entity"), Icon32x32));
	Style->Set("ArticyImporter.Type.FlowFragment.32", new IMAGE_BRUSH(TEXT("128/flowfragment"), Icon32x32));
	Style->Set("ArticyImporter.Type.Hub.32", new IMAGE_BRUSH(TEXT("128/hub"), Icon32x32));
	Style->Set("ArticyImporter.Type.Jump.32", new IMAGE_BRUSH(TEXT("128/jump"), Icon32x32));
	Style->Set("ArticyImporter.Type.Instruction.32", new IMAGE_BRUSH(TEXT("128/instruction"), Icon32x32));
	Style->Set("ArticyImporter.Type.Condition.32", new IMAGE_BRUSH(TEXT("128/condition"), Icon32x32));
	Style->Set("ArticyImporter.Type.Asset.32", new IMAGE_BRUSH(TEXT("128/asset"), Icon32x32));
	Style->Set("ArticyImporter.Type.UserFolder.32", new IMAGE_BRUSH(TEXT("128/assetfolder"), Icon32x32));
	Style->Set("ArticyImporter.Type.Zone.32", new IMAGE_BRUSH(TEXT("128/zone"), Icon32x32));
	Style->Set("ArticyImporter.Type.Location.32", new IMAGE_BRUSH(TEXT("128/location"), Icon32x32));
	Style->Set("ArticyImporter.Type.LocationImage.32", new IMAGE_BRUSH(TEXT("128/locationimage"), Icon32x32));
	Style->Set("ArticyImporter.Type.Link.32", new IMAGE_BRUSH(TEXT("128/link"), Icon32x32));
	Style->Set("ArticyImporter.Type.Comment.32", new IMAGE_BRUSH(TEXT("128/comment"), Icon32x32));

	Style->Set("ArticyImporter.Type.DialogueFragment.64", new IMAGE_BRUSH(TEXT("128/dialoguefragment"), Icon64x64));
	Style->Set("ArticyImporter.Type.Dialogue.64", new IMAGE_BRUSH(TEXT("128/dialogue2"), Icon64x64));
	Style->Set("ArticyImporter.Type.Entity.64", new IMAGE_BRUSH(TEXT("128/entity"), Icon64x64));
	Style->Set("ArticyImporter.Type.FlowFragment.64", new IMAGE_BRUSH(TEXT("128/flowfragment"), Icon64x64));
	Style->Set("ArticyImporter.Type.Hub.64", new IMAGE_BRUSH(TEXT("128/hub"), Icon64x64));
	Style->Set("ArticyImporter.Type.Jump.64", new IMAGE_BRUSH(TEXT("128/jump"), Icon64x64));
	Style->Set("ArticyImporter.Type.Instruction.64", new IMAGE_BRUSH(TEXT("128/instruction"), Icon64x64));
	Style->Set("ArticyImporter.Type.Condition.64", new IMAGE_BRUSH(TEXT("128/condition"), Icon64x64));
	Style->Set("ArticyImporter.Type.Asset.64", new IMAGE_BRUSH(TEXT("128/asset"), Icon64x64));
	Style->Set("ArticyImporter.Type.UserFolder.64", new IMAGE_BRUSH(TEXT("128/assetfolder"), Icon64x64));
	Style->Set("ArticyImporter.Type.Zone.64", new IMAGE_BRUSH(TEXT("128/zone"), Icon64x64));
	Style->Set("ArticyImporter.Type.Location.64", new IMAGE_BRUSH(TEXT("128/location"), Icon64x64));
	Style->Set("ArticyImporter.Type.LocationImage.64", new IMAGE_BRUSH(TEXT("128/locationimage"), Icon64x64));
	Style->Set("ArticyImporter.Type.Link.64", new IMAGE_BRUSH(TEXT("128/link"), Icon64x64));
	Style->Set("ArticyImporter.Type.Comment.64", new IMAGE_BRUSH(TEXT("128/comment"), Icon64x64));




	
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FArticyImporterStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FArticyImporterStyle::Get()
{
	return *StyleInstance;
}
