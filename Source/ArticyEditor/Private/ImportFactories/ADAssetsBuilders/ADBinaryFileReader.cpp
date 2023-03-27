//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//

#include "ADBinaryFileReader.h"
#include "ArticyEditor/Public/ArticyEditorModule.h"
#include "ArticyEditor/Private/ImportFactories/ADFileData/ArticyImportData.h"
#include "Misc/FileHelper.h"

void ADBinaryFileReader::ReadFile(TArray<uint8> Ar)
{
	// Header ------------------------------------------------------------------------------------------
	int IdentifierSize = sizeof(_requiredMagic) - 1; // less null terminated
	std::string cstr(reinterpret_cast<char*>(Ar.GetData()), IdentifierSize);
	const FString MAGIC(cstr.c_str());
	if(MAGIC !=_requiredMagic)
	{
		UE_LOG(LogArticyEditor,Error, TEXT("Archive type mismatch. Abort import."));
		return;
	}
	
	long long offset = IdentifierSize; // ... size of the magic number for initial read
	_version = GetFromBuffer(Ar,offset,sizeof(char));

	offset += sizeof(std::byte); // 1
	_pad = GetFromBuffer(Ar,offset,sizeof(char));

	offset += sizeof(std::byte); // 1
	_flags = GetFromBuffer(Ar,offset,sizeof(unsigned short));

	offset += sizeof(unsigned short);
	_fileNumber = GetFromBuffer(Ar, offset, sizeof(int));

	offset += sizeof(int);
	long long FileDictOffset = GetLongFromBuffer(Ar, offset, sizeof(long long));
	// File dictionary ------------------------------------------------------------------------------------------
	ADBinaryFileEntry FileEntry;
	for (int i = 0; i < _fileNumber; i++)
	{
		FileEntry = GetFileEntryFromArchive(FileDictOffset, Ar);
		FString JsonFile = ExtractJsonFromFileEntry(Ar, FileEntry);
		UE_LOG(LogArticyEditor, Warning, TEXT("------------------- JSON file #%i --------------------------------"), i+1);
		UE_LOG(LogArticyEditor, Warning, TEXT("%s"), *JsonFile);
		UE_LOG(LogArticyEditor, Warning, TEXT("------------------- ------------- --------------------------------"));

		// Dispatcher forwards data to exsting Json structures depending on filename.
		// Can be a chain of responsibility, strategy, Mediator + adapters, command stack... @Todo
		// _dispatcher.HandleJson(FileEntry.FileName, JsonFile);
		
		FileDictOffset += FileEntry.FileEntrysize;
	}
}

long long ADBinaryFileReader::GetLongFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize)
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

// Convenience. Avoid a cast for short/int types.
int ADBinaryFileReader::GetFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize)
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

TArray<uint8> ADBinaryFileReader::GetByteArrayFromBuffer(TArray<uint8> Buffer, int StartIndex, int Size)
{
	TArray<uint8> retArray;
	for (long long i = StartIndex; i < StartIndex + Size; i++)
		retArray.Add(Buffer[i]);

	return retArray;
}

FString ADBinaryFileReader::ExtractJsonFromFileEntry(TArray<uint8> Buffer, ADBinaryFileEntry fileEntry)
{
	TArray<uint8> bJSon = GetByteArrayFromBuffer(Buffer, fileEntry.FileStartPosition,
	                                                                  fileEntry.PackedLength);

	std::string retStr(reinterpret_cast<char*>(bJSon.GetData()), bJSon.Num());
	FString JSonFString(retStr.c_str());
	return JSonFString;
}

ADBinaryFileEntry ADBinaryFileReader::GetFileEntryFromArchive(long long offsetPosition, TArray<uint8> Ar)
{
	ADBinaryFileEntry retEntry;

	retEntry.FileStartPosition = GetLongFromBuffer(Ar, offsetPosition, sizeof(retEntry.FileStartPosition));
	offsetPosition += sizeof(retEntry.FileStartPosition);
	retEntry.UnpackedLength = GetLongFromBuffer(Ar, offsetPosition, sizeof(retEntry.UnpackedLength));

	offsetPosition += sizeof(retEntry.UnpackedLength);
	retEntry.PackedLength = GetLongFromBuffer(Ar, offsetPosition, sizeof(retEntry.PackedLength));

	offsetPosition += sizeof(retEntry.PackedLength);
	retEntry.Flags = GetFromBuffer(Ar, offsetPosition, sizeof(retEntry.Flags));

	offsetPosition += sizeof(retEntry.Flags);
	retEntry.UTF8NameLength = GetFromBuffer(Ar, offsetPosition, sizeof(retEntry.UTF8NameLength));

	offsetPosition += sizeof(retEntry.UTF8NameLength);
	retEntry.UTF8Filename = GetByteArrayFromBuffer(Ar, offsetPosition, retEntry.UTF8NameLength);

	std::string logStr(reinterpret_cast<char*>(retEntry.UTF8Filename.GetData()), retEntry.UTF8Filename.Num());
	const FString UELogStr(logStr.c_str());
	retEntry.FileName = UELogStr;	// Useful for dispatcher
	
	// There is probably far better than below ... but Anyway, this is optional ^^'
	// By example, enumerate each property size from a given Poco type using reflection
	// inside a static method... by the way, wouldn't this be an overkill ?...
	// => Refactor if the binary file entry structure changes too much.
	retEntry.FileEntrysize = sizeof(retEntry.FileStartPosition) +
		sizeof(retEntry.UnpackedLength) +
		sizeof(retEntry.PackedLength) +
		sizeof(retEntry.Flags) +
		sizeof(retEntry.UTF8NameLength) +
		retEntry.UTF8NameLength;

	return retEntry;
}