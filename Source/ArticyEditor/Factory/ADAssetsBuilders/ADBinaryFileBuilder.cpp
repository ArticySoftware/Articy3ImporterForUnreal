//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ADBinaryFileBuilder.h"
#include "ArticyEditor/ArticyEditor.h"
#include "ArticyEditor/Factory/ADFileData/AD_FileData.h"
#include "Misc/FileHelper.h"

void ADBinaryFileBuilder::BuildAsset(UAD_FileData& FileDataAsset, TArray<uint8> Ar)
{
	UE_LOG(ArticyEditor, Warning, TEXT("Reading binary archive..."));

	// Header ------------------------------------------------------------------------------------------
	int IdentifierSize = sizeof("ADFA") - 1;
	std::string cstr(reinterpret_cast<char*>(Ar.GetData()), IdentifierSize);
	const FString MAGIC(cstr.c_str());

	UE_LOG(ArticyEditor, Warning, TEXT("Found Magic = %s"), *MAGIC);

	long long offset = IdentifierSize; // ... size of the magic number for initial read
	UE_LOG(ArticyEditor, Warning, TEXT("Found version = %i"), GetFromBuffer(Ar,offset,sizeof(char)));

	offset += sizeof(std::byte); // 1
	UE_LOG(ArticyEditor, Warning, TEXT("Found pad = %i"), GetFromBuffer(Ar,offset,sizeof(char)));

	offset += sizeof(std::byte); // 1
	UE_LOG(ArticyEditor, Warning, TEXT("Found flags = %i"), GetFromBuffer(Ar,offset,sizeof(unsigned short)));

	offset += sizeof(unsigned short);
	int filesNumber = GetFromBuffer(Ar, offset, sizeof(int));
	UE_LOG(ArticyEditor, Warning, TEXT("Found number of files = %i"), filesNumber);

	offset += sizeof(int);
	long long FileDictOffset = GetLongFromBuffer(Ar, offset, sizeof(long long));
	UE_LOG(ArticyEditor, Warning, TEXT("Found file dictionary pos = %lld"), FileDictOffset);

	// File dictionary ------------------------------------------------------------------------------------------
	ADBinaryFileEntry FileEntry;
	for (int i = 0; i < filesNumber; i++)
	{
		UE_LOG(ArticyEditor, Warning, TEXT("----------------- Reading File entry header #%i ---------------------"),
		       i+1);
		FileEntry = GetFileEntryFromArchive(FileDictOffset, Ar);
		FString JsonFile = ExtractJsonFromFileEntry(Ar, FileEntry);
		UE_LOG(ArticyEditor, Warning, TEXT("------------------- JSON file #%i --------------------------------"), i+1);
		UE_LOG(ArticyEditor, Warning, TEXT("%s"), *JsonFile);
		UE_LOG(ArticyEditor, Warning, TEXT("------------------- ------------- --------------------------------"));

		// Dispatcher forwards data to exsting Json structures depending on filename.
		// Can be a chain of responsibility, strategy, Mediator + adapters, command stack... @Todo
		// _dispatcher.HandleJson(FileEntry.FileName, JsonFile);
		
		FileDictOffset += FileEntry.FileEntrysize;
	}
}

// ? => Remove (need only LP64 return value version, then cast...)
int ADBinaryFileBuilder::GetFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize)
{
	int int_value = 0;

	char* char_list = new char[typeSize];

	for (int i = 0; i < typeSize; i++)
	{
		char_list[i] = Buffer.GetData()[i + Offset];
	}

	FMemory::Memcpy(&int_value, char_list, typeSize);
	delete [] char_list;

	return int_value;
}

long long ADBinaryFileBuilder::GetLongFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize)
{
	long long long_value = 0;

	char* char_list = new char[typeSize];

	for (int i = 0; i < typeSize; i++)
	{
		char_list[i] = Buffer.GetData()[i + Offset];
	}

	FMemory::Memcpy(&long_value, char_list, typeSize);
	delete [] char_list;

	return long_value;
}


TArray<uint8> ADBinaryFileBuilder::GetByteArrayFromBuffer(TArray<uint8> Buffer, int StartIndex, int Size)
{
	TArray<uint8> retArray;
	for (long long i = StartIndex; i < StartIndex + Size; i++)
		retArray.Add(Buffer[i]);

	return retArray;
}

FString ADBinaryFileBuilder::ExtractJsonFromFileEntry(TArray<uint8> Buffer, ADBinaryFileEntry fileEntry)
{
	TArray<uint8> bJSon = GetByteArrayFromBuffer(Buffer, fileEntry.FileStartPosition,
	                                                                  fileEntry.PackedLength);

	std::string retStr(reinterpret_cast<char*>(bJSon.GetData()), bJSon.Num());
	FString JSonFString(retStr.c_str());
	return JSonFString;
}

ADBinaryFileEntry ADBinaryFileBuilder::GetFileEntryFromArchive(long long offsetPosition, TArray<uint8> Ar)
{
	ADBinaryFileEntry retEntry;

	retEntry.FileStartPosition = GetLongFromBuffer(Ar, offsetPosition, sizeof(retEntry.FileStartPosition));
	offsetPosition += sizeof(retEntry.FileStartPosition);
	retEntry.UnpackedLength = GetLongFromBuffer(Ar, offsetPosition, sizeof(retEntry.UnpackedLength));

	offsetPosition += sizeof(retEntry.UnpackedLength); //long long);
	retEntry.PackedLength = GetLongFromBuffer(Ar, offsetPosition, sizeof(retEntry.PackedLength));

	offsetPosition += sizeof(retEntry.PackedLength); //long long);
	retEntry.Flags = GetFromBuffer(Ar, offsetPosition, sizeof(retEntry.Flags));

	offsetPosition += sizeof(retEntry.Flags); //short);
	retEntry.UTF8NameLength = GetFromBuffer(Ar, offsetPosition, sizeof(retEntry.UTF8NameLength));

	offsetPosition += sizeof(retEntry.UTF8NameLength);
	retEntry.UTF8Filename = GetByteArrayFromBuffer(Ar, offsetPosition, retEntry.UTF8NameLength);

	std::string logStr(reinterpret_cast<char*>(retEntry.UTF8Filename.GetData()), retEntry.UTF8Filename.Num());
	const FString UELogStr(logStr.c_str());
	retEntry.FileName = UELogStr;
	UE_LOG(ArticyEditor, Warning, TEXT("File name : %s"),*retEntry.FileName);
	
	// There is probably far better than below ... but Anyway, this is optional ^^'
	// (enumerate each property size from a given Poco type using reflection inside a static method...
	// wouldn't this be an overkill ?...)
	retEntry.FileEntrysize = sizeof(retEntry.FileStartPosition) +
		sizeof(retEntry.UnpackedLength) +
		sizeof(retEntry.PackedLength) +
		sizeof(retEntry.Flags) +
		sizeof(retEntry.UTF8NameLength) +
		retEntry.UTF8NameLength;

	return retEntry;
}