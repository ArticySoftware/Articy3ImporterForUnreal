#pragma once

class UAD_FileData;

struct ADBinaryFileEntry
{
public:
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

	// Convenient but optional
	long long FileEntrysize;
	// -----------------------

	// (?) Move to ADBinaryFileBuilder if ADBinaryFileEntry 
	// is considered as a POCO... (@todo archi)
	static ADBinaryFileEntry GetFileEntryFromArchive(long long offsetPosition, TArray<uint8> Ar);
};

class ADBinaryFileBuilder
{
public:
	static void BuildAsset(UAD_FileData& FileDataAsset, TArray<uint8> Ar);
	
	static int GetFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize);
	static long long GetLongFromBuffer(TArray<uint8> Buffer, int Offset, int typeSize);
	static TArray<uint8> GetByteArrayFromBuffer(TArray<uint8> Buffer, int Offset, int Size);
	static FString ExtractJsonFromFileEntry(TArray<uint8> Buffer, ADBinaryFileEntry fileEntry);
};
