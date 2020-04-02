//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
 
//

#include "ArticyEditorStyle.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FArticyEditorStyle::StyleInstance = NULL;

void FArticyEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FArticyEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FArticyEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ArticyEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon8x8(8.0f, 8.0f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);
const FVector2D Icon96x96(96.0f, 96.0f);

TSharedRef< FSlateStyleSet > FArticyEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ArticyEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ArticyImporter")->GetBaseDir() / TEXT("Resources"));

	// the default icon for the UICommand "OpenPluginWindow"
	Style->Set("ArticyImporter.OpenPluginWindow", new IMAGE_BRUSH(TEXT("ButtonIcon_40x"), Icon40x40));

	Style->Set("ArticyImporter.ArticyDraftLogoText", new IMAGE_BRUSH(TEXT("ArticyDraftLogoText"), FVector2D(291, 54)));
	Style->Set("ArticyImporter.ArticyDraft.8", new IMAGE_BRUSH(TEXT("ArticyDraft16"), Icon8x8));
	Style->Set("ArticyImporter.ArticyDraft.16", new IMAGE_BRUSH(TEXT("ArticyDraft16"), Icon16x16));
	Style->Set("ArticyImporter.ArticyDraft.32", new IMAGE_BRUSH(TEXT("ArticyDraft32"), Icon32x32));
	
	Style->Set("ArticyImporter.ArticySoftware.16", new IMAGE_BRUSH(TEXT("ArticySoftware16"), Icon16x16));
	Style->Set("ArticyImporter.ArticySoftware.32", new IMAGE_BRUSH(TEXT("ArticySoftware32"), Icon32x32));
	Style->Set("ArticyImporter.ArticySoftware.40", new IMAGE_BRUSH(TEXT("ArticySoftware40"), Icon40x40));
	Style->Set("ArticyImporter.ArticySoftware.64", new IMAGE_BRUSH(TEXT("ArticySoftware64"), Icon64x64));

	Style->Set("ArticyImporter.ArticyImporter.8", new IMAGE_BRUSH(TEXT("ArticyImporter64"), Icon8x8));
	Style->Set("ArticyImporter.ArticyImporter.16", new IMAGE_BRUSH(TEXT("ArticyImporter64"), Icon16x16));
	Style->Set("ArticyImporter.ArticyImporter.40", new IMAGE_BRUSH(TEXT("ArticyImporter40"), Icon40x40));
	Style->Set("ArticyImporter.ArticyImporter.64", new IMAGE_BRUSH(TEXT("ArticyImporter64"), Icon64x64));

	Style->Set("ArticyImporter.Window.ImporterLogo", new IMAGE_BRUSH(TEXT("ArticyWindow/Logo_ArticyDraftImporter_TopMiddle"), FVector2D(404, 125)));
	Style->Set("ArticyImporter.Window.ArticyLogo", new IMAGE_BRUSH(TEXT("ArticyWindow/Logo_Articy_BottomRightCropped"), FVector2D(170, 57)));


	FButtonStyle FullReimportButtonStyle = FButtonStyle()
		.SetNormal(IMAGE_BRUSH("ArticyWindow/Button_FullReimport", FVector2D(333, 101)))
		.SetHovered(IMAGE_BRUSH("ArticyWindow/Button_FullReimport_Hover", FVector2D(333, 101)))
		.SetPressed(IMAGE_BRUSH("ArticyWindow/Button_FullReimport_Press", FVector2D(333, 101)));

	FButtonStyle ImportChangesButtonStyle = FButtonStyle()
		.SetNormal(IMAGE_BRUSH("ArticyWindow/Button_ImportChanges", FVector2D(333, 101)))
		.SetHovered(IMAGE_BRUSH("ArticyWindow/Button_ImportChanges_Hover", FVector2D(333, 101)))
		.SetPressed(IMAGE_BRUSH("ArticyWindow/Button_ImportChanges_Press", FVector2D(333, 101)));

	FButtonStyle RegenerateAssetsButtonStyle = FButtonStyle()
		.SetNormal(IMAGE_BRUSH("ArticyWindow/Button_RegenerateAssets", FVector2D(333, 101)))
		.SetHovered(IMAGE_BRUSH("ArticyWindow/Button_RegenerateAssets_Hover", FVector2D(333, 101)))
		.SetPressed(IMAGE_BRUSH("ArticyWindow/Button_RegenerateAssets_Press", FVector2D(333, 101)));
	
	Style->Set("ArticyImporter.Button.FullReimport", FullReimportButtonStyle);
	Style->Set("ArticyImporter.Button.ImportChanges", ImportChangesButtonStyle);
	Style->Set("ArticyImporter.Button.RegenerateAssets", RegenerateAssetsButtonStyle);

	Style->Set("ArticyImporter.AssetPicker.TileBackground.96", new IMAGE_BRUSH(TEXT("128/content_tile_fill"), Icon96x96));
	Style->Set("ArticyImporter.AssetPicker.TileBorder.16", new BOX_BRUSH(TEXT("Border"), 4.f / 16.f));
	
	Style->Set("ArticyImporter.Type.DialogueFragment.16", new IMAGE_BRUSH(TEXT("128/dialoguefragment"), Icon16x16));
	Style->Set("ArticyImporter.Type.Dialogue.16", new IMAGE_BRUSH(TEXT("128/dialogue2"), Icon16x16));
	Style->Set("ArticyImporter.Type.Entity.16", new IMAGE_BRUSH(TEXT("128/entity"), Icon16x16));
	Style->Set("ArticyImporter.Type.FlowFragment.16", new IMAGE_BRUSH(TEXT("128/flowfragment"), Icon16x16));
	Style->Set("ArticyImporter.Type.Hub.16", new IMAGE_BRUSH(TEXT("128/hub"), Icon16x16));
	Style->Set("ArticyImporter.Type.Jump.16", new IMAGE_BRUSH(TEXT("128/jump"), Icon16x16));
	Style->Set("ArticyImporter.Type.Instruction.16", new IMAGE_BRUSH(TEXT("128/instruction"), Icon16x16));
	Style->Set("ArticyImporter.Type.Condition.16", new IMAGE_BRUSH(TEXT("128/condition"), Icon16x16));
	Style->Set("ArticyImporter.Type.Asset.16", new IMAGE_BRUSH(TEXT("128/asset"), Icon16x16));
	Style->Set("ArticyImporter.Type.UserFolder.16", new IMAGE_BRUSH(TEXT("128/userfolder"), Icon16x16));
	Style->Set("ArticyImporter.Type.Zone.16", new IMAGE_BRUSH(TEXT("128/zone"), Icon16x16));
	Style->Set("ArticyImporter.Type.Location.16", new IMAGE_BRUSH(TEXT("128/location"), Icon16x16));
	Style->Set("ArticyImporter.Type.LocationImage.16", new IMAGE_BRUSH(TEXT("128/locationimage"), Icon16x16));
	Style->Set("ArticyImporter.Type.Link.16", new IMAGE_BRUSH(TEXT("128/link"), Icon16x16));
	Style->Set("ArticyImporter.Type.Comment.16", new IMAGE_BRUSH(TEXT("128/comment"), Icon16x16));
	Style->Set("ArticyImporter.Type.TextObject.16", new IMAGE_BRUSH(TEXT("128/textobject"), Icon16x16));
	Style->Set("ArticyImporter.Type.LocationText.16", new IMAGE_BRUSH(TEXT("128/locationtext"), Icon16x16));
	Style->Set("ArticyImporter.Type.Document.16", new IMAGE_BRUSH(TEXT("128/document"), Icon16x16));
	Style->Set("ArticyImporter.Type.Spot.16", new IMAGE_BRUSH(TEXT("256/spot"), Icon16x16));
	Style->Set("ArticyImporter.Type.Path.16", new IMAGE_BRUSH(TEXT("256/path"), Icon16x16));


	
	Style->Set("ArticyImporter.Type.DialogueFragment.32", new IMAGE_BRUSH(TEXT("128/dialoguefragment"), Icon32x32));
	Style->Set("ArticyImporter.Type.Dialogue.32", new IMAGE_BRUSH(TEXT("128/dialogue2"), Icon32x32));
	Style->Set("ArticyImporter.Type.Entity.32", new IMAGE_BRUSH(TEXT("128/entity"), Icon32x32));
	Style->Set("ArticyImporter.Type.FlowFragment.32", new IMAGE_BRUSH(TEXT("128/flowfragment"), Icon32x32));
	Style->Set("ArticyImporter.Type.Hub.32", new IMAGE_BRUSH(TEXT("128/hub"), Icon32x32));
	Style->Set("ArticyImporter.Type.Jump.32", new IMAGE_BRUSH(TEXT("128/jump"), Icon32x32));
	Style->Set("ArticyImporter.Type.Instruction.32", new IMAGE_BRUSH(TEXT("128/instruction"), Icon32x32));
	Style->Set("ArticyImporter.Type.Condition.32", new IMAGE_BRUSH(TEXT("128/condition"), Icon32x32));
	Style->Set("ArticyImporter.Type.Asset.32", new IMAGE_BRUSH(TEXT("128/asset"), Icon32x32));
	Style->Set("ArticyImporter.Type.UserFolder.32", new IMAGE_BRUSH(TEXT("128/userfolder"), Icon32x32));
	Style->Set("ArticyImporter.Type.Zone.32", new IMAGE_BRUSH(TEXT("128/zone"), Icon32x32));
	Style->Set("ArticyImporter.Type.Location.32", new IMAGE_BRUSH(TEXT("128/location"), Icon32x32));
	Style->Set("ArticyImporter.Type.LocationImage.32", new IMAGE_BRUSH(TEXT("128/locationimage"), Icon32x32));
	Style->Set("ArticyImporter.Type.Link.32", new IMAGE_BRUSH(TEXT("128/link"), Icon32x32));
	Style->Set("ArticyImporter.Type.Comment.32", new IMAGE_BRUSH(TEXT("128/comment"), Icon32x32));
	Style->Set("ArticyImporter.Type.TextObject.32", new IMAGE_BRUSH(TEXT("128/textobject"), Icon32x32));
	Style->Set("ArticyImporter.Type.LocationText.32", new IMAGE_BRUSH(TEXT("128/locationtext"), Icon32x32));
	Style->Set("ArticyImporter.Type.Document.32", new IMAGE_BRUSH(TEXT("128/document"), Icon32x32));
	Style->Set("ArticyImporter.Type.Spot.32", new IMAGE_BRUSH(TEXT("256/spot"), Icon32x32));
	Style->Set("ArticyImporter.Type.Path.32", new IMAGE_BRUSH(TEXT("256/path"), Icon32x32));


	Style->Set("ArticyImporter.Type.DialogueFragment.64", new IMAGE_BRUSH(TEXT("128/dialoguefragment"), Icon64x64));
	Style->Set("ArticyImporter.Type.Dialogue.64", new IMAGE_BRUSH(TEXT("128/dialogue2"), Icon64x64));
	Style->Set("ArticyImporter.Type.Entity.64", new IMAGE_BRUSH(TEXT("128/entity"), Icon64x64));
	Style->Set("ArticyImporter.Type.FlowFragment.64", new IMAGE_BRUSH(TEXT("128/flowfragment"), Icon64x64));
	Style->Set("ArticyImporter.Type.Hub.64", new IMAGE_BRUSH(TEXT("128/hub"), Icon64x64));
	Style->Set("ArticyImporter.Type.Jump.64", new IMAGE_BRUSH(TEXT("128/jump"), Icon64x64));
	Style->Set("ArticyImporter.Type.Instruction.64", new IMAGE_BRUSH(TEXT("128/instruction"), Icon64x64));
	Style->Set("ArticyImporter.Type.Condition.64", new IMAGE_BRUSH(TEXT("128/condition"), Icon64x64));
	Style->Set("ArticyImporter.Type.Asset.64", new IMAGE_BRUSH(TEXT("128/asset"), Icon64x64));
	Style->Set("ArticyImporter.Type.UserFolder.64", new IMAGE_BRUSH(TEXT("128/userfolder"), Icon64x64));
	Style->Set("ArticyImporter.Type.Zone.64", new IMAGE_BRUSH(TEXT("128/zone"), Icon64x64));
	Style->Set("ArticyImporter.Type.Location.64", new IMAGE_BRUSH(TEXT("128/location"), Icon64x64));
	Style->Set("ArticyImporter.Type.LocationImage.64", new IMAGE_BRUSH(TEXT("128/locationimage"), Icon64x64));
	Style->Set("ArticyImporter.Type.Link.64", new IMAGE_BRUSH(TEXT("128/link"), Icon64x64));
	Style->Set("ArticyImporter.Type.Comment.64", new IMAGE_BRUSH(TEXT("128/comment"), Icon64x64));
	Style->Set("ArticyImporter.Type.TextObject.64", new IMAGE_BRUSH(TEXT("128/textobject"), Icon64x64));
	Style->Set("ArticyImporter.Type.LocationText.64", new IMAGE_BRUSH(TEXT("128/locationtext"), Icon64x64));
	Style->Set("ArticyImporter.Type.Document.64", new IMAGE_BRUSH(TEXT("128/document"), Icon64x64));
	Style->Set("ArticyImporter.Type.Spot.64", new IMAGE_BRUSH(TEXT("256/spot"), Icon64x64));
	Style->Set("ArticyImporter.Type.Path.64", new IMAGE_BRUSH(TEXT("256/path"), Icon64x64));

	
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FArticyEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FArticyEditorStyle::Get()
{
	return *StyleInstance;
}
