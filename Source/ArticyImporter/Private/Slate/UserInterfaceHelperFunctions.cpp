//  
// Copyright (c) articy Software GmbH & Co. KG. All rights reserved.  
// Licensed under the MIT license. See LICENSE file in the project root for full license information.  
//


#include "Slate/UserInterfaceHelperFunctions.h"
#include "ArticyObject.h"
#include "Interfaces/ArticyObjectWithPreviewImage.h"
#include "ArticyAsset.h"
#include "Interfaces/ArticyObjectWithSpeaker.h"
#include "Interfaces/ArticyObjectWithDisplayName.h"
#include "Interfaces/ArticyObjectWithText.h"
#include "Kismet/KismetStringLibrary.h"
#include "ArticyImportData.h"
#include "ArticyImporterStyle.h"
#include "ArticyFlowClasses.h"
#include "ArticyEntity.h"
#include "Interfaces/ArticyObjectWithColor.h"

const FSlateBrush* UserInterfaceHelperFunctions::GetArticyTypeImage(const UArticyObject* ArticyObject, UserInterfaceHelperFunctions::EImageSize Size)
{
	if(!ArticyObject)
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.ArticyApplication.64")));
	}

	FString SizeString = "";
	if(Size == EImageSize::Large)
	{
		SizeString.Append("64");
	}
	else if(Size == EImageSize::Medium)
	{
		SizeString.Append("32");
	}
	else
	{
		SizeString.Append("16");
	}
	
	if(ArticyObject->IsA<UArticyDialogueFragment>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.DialogueFragment.").Append(SizeString)));
	}
	else if(ArticyObject->IsA<UArticyDialogue>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Dialogue.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyEntity>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Entity.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyFlowFragment>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.FlowFragment.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyHub>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Hub.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyJump>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Jump.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyInstruction>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Instruction.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyCondition>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Condition.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyAsset>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Asset.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyUserFolder>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.UserFolder.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyZone>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Zone.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyLocation>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Location.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyLocationImage>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.LocationImage.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyLocationText>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.LocationText.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyLink>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Link.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyComment>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Comment.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyTextObject>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.TextObject.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyPath>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Path.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticyDocument>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Document.").Append(SizeString)));
	}
	else if (ArticyObject->IsA<UArticySpot>())
	{
		return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.Type.Spot.").Append(SizeString)));
	}

	return FArticyImporterStyle::Get().GetBrush(FName(*FString("ArticyImporter.ArticyApplication.64")));
}

UTexture2D* UserInterfaceHelperFunctions::GetDisplayImage(const UArticyObject* ArticyObject)
{
	UTexture2D* Image = nullptr;
	
	if (!ArticyObject)
	{
		return Image;
	}

	// if there is a preview image, use it as display image
	const IArticyObjectWithPreviewImage* ObjectWithPreviewImage = Cast<IArticyObjectWithPreviewImage>(ArticyObject);
	if (ObjectWithPreviewImage)
	{
		const FArticyId AssetID = ObjectWithPreviewImage->GetPreviewImage()->Asset;
		const UArticyObject* AssetObject = UArticyObject::FindAsset(AssetID);

		if (AssetObject)
		{
			Image = (Cast<UArticyAsset>(AssetObject))->LoadAsTexture2D();
			return Image;
		}
	}

	return Image;
}

bool UserInterfaceHelperFunctions::RetrievePreviewImage(const UArticyObject* ArticyObject, FSlateBrush& OutSlateBrush)
{
	UTexture2D* PreviewImage = GetDisplayImage(ArticyObject);

	if(PreviewImage)
	{
		OutSlateBrush.SetResourceObject(PreviewImage);
		return true;
	}

	return false;
}

bool UserInterfaceHelperFunctions::RetrieveSpeakerPreviewImage(const UArticyObject* ArticyObject, FSlateBrush& OutSlateBrush)
{
	const IArticyObjectWithSpeaker* ArticyObjectWithSpeaker = Cast<IArticyObjectWithSpeaker>(ArticyObject);

	if(ArticyObjectWithSpeaker)
	{
		UArticyObject* Speaker = UArticyObject::FindAsset(ArticyObjectWithSpeaker->GetSpeakerId());
		return RetrievePreviewImage(Speaker, OutSlateBrush);
	}

	return false;
}

const FArticyId* UserInterfaceHelperFunctions::GetTargetID(const UArticyObject* ArticyObject)
{
	if(!ArticyObject)
	{
		return nullptr; 
	}
	
	static const auto PropName = FName("Target");
	UProperty* Prop = ArticyObject->GetProperty(PropName);

	if(!Prop)
	{
		return nullptr;
	}
	
	const FArticyId* TargetID = Prop->ContainerPtrToValuePtr<FArticyId>(ArticyObject);

	if(TargetID)
	{
		return TargetID;
	}

	return nullptr;
}

const FString UserInterfaceHelperFunctions::GetDisplayName(const UArticyObject* ArticyObject)
{
	FString DisplayName = "None";
	
	if (!ArticyObject)
	{
		return DisplayName;
	}
	
	const FArticyId* TargetID = UserInterfaceHelperFunctions::GetTargetID(ArticyObject);
	if (TargetID)
	{
		const UArticyObject* TargetObject = UArticyObject::FindAsset(*TargetID);

		if (TargetObject)
		{
			return GetDisplayName(TargetObject);
		}
	}
	
	// use the display name as display name, if available
	const IArticyObjectWithDisplayName* ArticyObjectWithDisplayName = Cast<IArticyObjectWithDisplayName>(ArticyObject);
	if (ArticyObjectWithDisplayName)
	{
		 DisplayName = ArticyObjectWithDisplayName->GetDisplayName().ToString();

		if (!DisplayName.IsEmpty())
		{
			return DisplayName;
		}
	}

	// use a text preview as display name, if available
	const IArticyObjectWithText* ArticyObjectWithText = Cast<IArticyObjectWithText>(ArticyObject);
	if (ArticyObjectWithText)
	{
		const FText Text = ArticyObjectWithText->GetText();

		// cut the string to the first X characters and attach "..." as a preview
		// primarily used for dialogue fragments, as they have neither preview image nor display name
		if (!Text.IsEmpty())
		{
			DisplayName = Text.ToString();
			DisplayName = UKismetStringLibrary::GetSubstring(DisplayName, 0, 14);

			if (Text.ToString().Len() > 14)
			{
				DisplayName.Append("...");
			}

			return DisplayName;
		}
	}
	
	return DisplayName;
}

const FLinearColor UserInterfaceHelperFunctions::GetColor(const UArticyObject* ArticyObject)
{
	FLinearColor Color(0.577, 0.76, 0.799);


	if (!ArticyObject)
	{
		return Color;
	}

	const FArticyId* TargetID = UserInterfaceHelperFunctions::GetTargetID(ArticyObject);

	if (TargetID)
	{
		const UArticyObject* TargetObject = UArticyObject::FindAsset(*TargetID);

		if (TargetObject)
		{
			return GetColor(TargetObject);
		}
	}
	
	const IArticyObjectWithColor* ObjectWithColor = Cast<IArticyObjectWithColor>(ArticyObject);
	if(ObjectWithColor)
	{
		Color = ObjectWithColor->GetColor();
		return Color;
	}	
	
	return Color;
}

const bool UserInterfaceHelperFunctions::ShowObjectInArticy(const UArticyObject* ArticyObject)
{
	if(ArticyObject== nullptr)
	{
		return false;
	}
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> OutAssetData;
	AssetRegistryModule.Get().GetAssetsByClass(UArticyImportData::StaticClass()->GetFName(), OutAssetData, false);

	if(OutAssetData.Num() == 1)
	{
		UArticyImportData* ImportData = Cast<UArticyImportData>(OutAssetData[0].GetAsset());
		FString ProjectGuid = ImportData->GetProject().Guid;

		FArticyId ArticyId = ArticyObject->GetId();

		// restore the original Id in decimal
		const int32 High = ArticyId.High;
		const int32 Low = ArticyId.Low;
		const uint64 HighOriginal = (uint64) High <<  32;
		const uint64 LowOriginal = Low;
		const uint64 OriginalId = HighOriginal + LowOriginal;
		
		FString ArticyObjectId = FString::Printf(TEXT("%llu"), OriginalId);
		FString ArticyURL = FString::Printf(TEXT("articy://localhost/view/%s/%s?pane=selected&tab=current"), *ProjectGuid, *ArticyObjectId);
		UKismetSystemLibrary::LaunchURL(ArticyURL);
	}
	
	return true;
}
