//  
// Copyright (c) Articy Software GmbH & Co. KG. All rights reserved.  
//
#pragma once

// External forward
class UAD_FileData;

// Convenience (local file) forward
struct ADBinaryFileEntry;

/**
 * @brief Read All sections from a multi-Json file binary archive.
 *
 * @DevNote :
 *		  * This class don't build anything (appart in-memory structures from input data)
 *		  => @Todo rename to ADBinaryArchiveReader + rename Main method (BuildAsset to ReadBytes)
 *
 *		  * Static methods prevent DI for strategy implementation (data forwarding, adaptor, etc..)
 *			and testability
 *		  => @todo create object instance instead of static methods lib,
 *				then pass-in some strategy pattern from constructor by DI
 *				(JSonFilesDispatcher, JSonFilesAdapter... etc...) 
 *		  
 */
class ADBinaryFileBuilder
{
	/*
private:
	static IJSonFileDispatcher _dispatcher;
	*/
	
public:
	// static void PH_Constructor(IJSonFileDispatcher _dispatcher);
	
	// Bad name (see @brief) => Rename to "PopulateContext", "DoStuff", "Execute", "ReadFile".... 
	static void BuildAsset(UAD_FileData& FileDataAsset, TArray<uint8> Ar);

private:
	static int GetFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize);
	static long long GetLongFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize);
	static TArray<uint8> GetByteArrayFromBuffer(TArray<uint8> Buffer, int Offset, int Size);
	static FString ExtractJsonFromFileEntry(TArray<uint8> Buffer, ADBinaryFileEntry fileEntry);

	static ADBinaryFileEntry GetFileEntryFromArchive(long long offsetPosition, TArray<uint8> Ar);
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