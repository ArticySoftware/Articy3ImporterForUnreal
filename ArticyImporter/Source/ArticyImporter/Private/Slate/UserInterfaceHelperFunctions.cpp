// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterfaceHelperFunctions.h"
#include "ArticyObject.h"
#include "ArticyObjectWithPreviewImage.h"
#include "ArticyAsset.h"
#include "ArticyObjectWithSpeaker.h"
#include "ArticyObjectWithDisplayName.h"
#include "ArticyObjectWithText.h"
#include "Kismet/KismetStringLibrary.h"
#include "ArticyImportData.h"

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

	// if we don't have a preview image, use the speaker image if available
	const IArticyObjectWithSpeaker* ObjectWithSpeaker = Cast<IArticyObjectWithSpeaker>(ArticyObject);
	if (ObjectWithSpeaker)
	{
		UArticyObject* SpeakerObject = UArticyObject::FindAsset(ObjectWithSpeaker->GetSpeakerId());
		const IArticyObjectWithPreviewImage* SpeakerWithImage = Cast<IArticyObjectWithPreviewImage>(SpeakerObject);

		if (SpeakerWithImage)
		{
			const FArticyId AssetID = SpeakerWithImage->GetPreviewImage()->Asset;
			const UArticyObject* AssetObject = UArticyObject::FindAsset(AssetID);

			if (AssetObject)
			{
				Image = (Cast<UArticyAsset>(AssetObject))->LoadAsTexture2D();
				return Image;
			}
		}
	}
	
	return Image;
}

const FString UserInterfaceHelperFunctions::GetDisplayName(const UArticyObject* ArticyObject)
{
	FString DisplayName = "None";
	
	if (!ArticyObject)
	{
		return DisplayName;
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
