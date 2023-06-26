// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ArticyArchiveReader.generated.h"

USTRUCT()
struct ARTICYEDITOR_API FArticyArchiveHeader
{
	GENERATED_BODY()
	
	FString Magic;
	uint8 Version;
	uint8 Pad;
	uint16 Flags;
	int32 NumberOfFiles;
	uint64 FileDictionaryPos;
};

USTRUCT()
struct ARTICYEDITOR_API FArticyArchiveFileData
{
	GENERATED_BODY()

	uint64 FileStartPos;
	int64 UnpackedLength;
	int64 PackedLength;
	int16 Flags;
	FString Filename;
};

UCLASS()
class ARTICYEDITOR_API UArticyArchiveReader : public UObject
{
	GENERATED_BODY()

public:
	bool OpenArchive(const FString& InArchiveFileName);
	bool ReadFile(const FString& Filename, FString& OutResult) const;

	static FString ArchiveBytesToString(const uint8* In, int32 Count);
	bool FetchJson(
		const TSharedPtr<FJsonObject>& JsonRoot,
		const FString& FieldName,
		FString& Hash,
		TSharedPtr<FJsonObject>& OutJsonObject) const;

protected:
	bool ReadHeader();
	bool ReadFileData();

	FString ArchiveFileName;
	FArticyArchiveHeader Header;
	TMap<FString, FArticyArchiveFileData> FileDictionary;
};
