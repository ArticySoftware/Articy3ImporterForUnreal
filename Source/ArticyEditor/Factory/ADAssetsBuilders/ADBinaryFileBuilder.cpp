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

	// Header
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
	int filesNumber =  GetFromBuffer(Ar,offset,sizeof(int));
	UE_LOG(ArticyEditor, Warning, TEXT("Found number of files = %i"),filesNumber);

	offset += sizeof(int);
	long long FileDictOffset = GetLongFromBuffer(Ar, offset, sizeof(long long));
	UE_LOG(ArticyEditor, Warning, TEXT("Found file dictionary pos = %lld"), FileDictOffset);

	// File dictionary
	ADBinaryFileEntry FileEntry;
	for(int i = 0; i <filesNumber;i++ )
	{
		UE_LOG(ArticyEditor, Warning, TEXT("----------------- Reading File entry header #%i ---------------------"),i+1);
		FileEntry = ADBinaryFileEntry::GetFileEntryFromArchive(FileDictOffset, Ar);
		FString JsonFile = ExtractJsonFromFileEntry(Ar,FileEntry);
		UE_LOG(ArticyEditor, Warning, TEXT("------------------- JSON file #%i --------------------------------"),i+1);
		UE_LOG(ArticyEditor, Warning, TEXT("%s"), *JsonFile);
		UE_LOG(ArticyEditor, Warning, TEXT("------------------- ------------- --------------------------------"));
		FileDictOffset+= FileEntry.FileEntrysize;
	}
}

ADBinaryFileEntry ADBinaryFileEntry::GetFileEntryFromArchive(long long offsetPosition, TArray<uint8> Ar)
{
	ADBinaryFileEntry retEntry;
	
	retEntry.FileStartPosition = ADBinaryFileBuilder::GetLongFromBuffer(Ar, offsetPosition, sizeof(long long));
	UE_LOG(ArticyEditor, Warning, TEXT("File start position : %li"),retEntry.FileStartPosition);

	offsetPosition+=sizeof(long long);
	UE_LOG(ArticyEditor, Warning,TEXT(" offset : %li, sizeof(long) : %llu"),offsetPosition, sizeof(long long));
	retEntry.UnpackedLength  = ADBinaryFileBuilder::GetLongFromBuffer(Ar, offsetPosition, sizeof(long));
	UE_LOG(ArticyEditor, Warning, TEXT("File UnpackedLength : %li"),retEntry.UnpackedLength);

	offsetPosition+=sizeof(long long);
	retEntry.PackedLength   = ADBinaryFileBuilder::GetLongFromBuffer(Ar, offsetPosition, sizeof(long long));
	UE_LOG(ArticyEditor, Warning, TEXT("File packedLength : %li"),retEntry.PackedLength);

	offsetPosition+=sizeof(long long);
	retEntry.Flags   = ADBinaryFileBuilder::GetFromBuffer(Ar, offsetPosition, sizeof(short));
	UE_LOG(ArticyEditor, Warning, TEXT("File Flags : %li"),retEntry.Flags);

	offsetPosition+=sizeof(short);
	retEntry.UTF8NameLength   = ADBinaryFileBuilder::GetFromBuffer(Ar, offsetPosition, sizeof(short));
	UE_LOG(ArticyEditor, Warning, TEXT("File UTF8NameLength : %li"),retEntry.UTF8NameLength);

	offsetPosition+= sizeof(short);
	retEntry.UTF8Filename = ADBinaryFileBuilder::GetByteArrayFromBuffer(Ar,offsetPosition,retEntry.UTF8NameLength);
		 
	std::string logStr(reinterpret_cast<char*>(retEntry.UTF8Filename.GetData()), retEntry.UTF8Filename.Num());
	const FString UELogStr(logStr.c_str());
	UE_LOG(ArticyEditor, Warning, TEXT("File name : %s"),*UELogStr);

	// probably better below ... but Anyway, this is optional ^^'
	// sizeof(ADBinaryFileEntry) + retEntry.UTF8NameLength
	retEntry.FileEntrysize = sizeof(long long) * 3 + sizeof(short) * 2 + retEntry.UTF8NameLength;
	
	return retEntry;
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
	for(long long i = StartIndex; i<StartIndex+Size;i++)
		retArray.Add(Buffer[i]);

	return retArray;
}

// From ADBinaryFileBuilder => Move to another .h/.cpp files 
FString ADBinaryFileBuilder::ExtractJsonFromFileEntry(TArray<uint8> Buffer, ADBinaryFileEntry fileEntry)
{
	TArray<uint8> bJSon = ADBinaryFileBuilder::GetByteArrayFromBuffer(Buffer,fileEntry.FileStartPosition ,fileEntry.PackedLength);
		 
	std::string retStr(reinterpret_cast<char*>(bJSon.GetData()), bJSon.Num());
	FString JSonFString(retStr.c_str());
	return JSonFString;
}
