//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//
#pragma once
#include "FileDispatcher/IFileDispatcher.h"


// External forward
class UAD_FileData;

// Convenience (local file) forward
struct ADBinaryFileEntry;

/**
 * @brief Read All sections from a multi-Json file binary archive.		  
 */
class ADBinaryFileReader
{
	const char _requiredMagic[5] = "ADFA"; // 4 + null terminated
	
	// Prioritary properties
	TUniquePtr<IFileDispatcher> _dispatcher;
	int _fileNumber = 0;
	
	// Secondary properties (convenience - not used right now - @warning : Anchor?)
	uint8 _version = 0;
	uint8 _pad = 0;
	uint8 _flags = 0;
	
public:
	ADBinaryFileReader(IFileDispatcher* Dispatcher)
	{
		// Transfert ownership... UniquePtr or SharedPtr ?..
		_dispatcher = MakeUnique<IFileDispatcher>(*Dispatcher);
	}
	
	/**
	 * @brief Read the content of the provided byte array. 
	 * @param Ar byte data array representing an AD. binary archive.
	 */
	void ReadFile(TArray<uint8> Ar);

private:
	long long GetLongFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize);
	int GetFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize); // Convenience. Avoid a cast for short/int types.
	TArray<uint8> GetByteArrayFromBuffer(TArray<uint8> Buffer, int Offset, int Size);
	FString ExtractJsonFromFileEntry(TArray<uint8> Buffer, ADBinaryFileEntry fileEntry);

	ADBinaryFileEntry GetFileEntryFromArchive(long long offsetPosition, TArray<uint8> Ar);
};

/**
 * @brief POCO file entry description read from binary file dictionary.
 */
struct ADBinaryFileEntry
{
	// LP32 vc LP64 long type interpretation.
	// => current test case version is LP64
	// (not LLP64 => Unix like os compatibility)
	// So casting to long long instead of long
	long long FileStartPosition;
	long long UnpackedLength;
	long long PackedLength;
	short Flags;
	short UTF8NameLength;
	TArray<uint8> UTF8Filename;
	FString FileName;
	
	// Convenient but optional
	long long FileEntrysize;
};