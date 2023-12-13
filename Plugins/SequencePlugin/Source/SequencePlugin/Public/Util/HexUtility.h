#pragma once
#include "Types/Types.h"

static const int32 HexLUTSize = 16;
static const FString HexLUTNew[] = {"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};

//used in many places
FString IntToHexString(uint64 Num);

//only used internally by IntToHexString
FString IntToHexLetter(uint8 Num);

//TOptional<uint8> HexLetterToInt(FString Hex); never used
TOptional<uint64> HexStringToUint64(FString Hex); //used only a handful of times not sure if TOpt is needed though

/*
* Used to take a HexString and convert it into a list of hex chars
*/
//TArray<uint8_t> HexToHexCharList(FString in);

int32 GetBytesInString(FString in);

uint8_t GetPaddingByte(int32 StringLength);

TArray<uint8_t> PKCS7(FString in);

/*
* Generates a random hex character
*/
FString RandomHexCharacter();

/*
* Generates a random hex byte
*/
FString RandomHexByte();

/*
* Generates a random Byte
*/
uint8 RandomByte();

//these functions are to take BytesToHex (and should be replaced by unreal versions under the hood)
//FString HashToHexString(ByteLength Size, Hash Hash);
//Hash HexStringToBytes(ByteLength Size, FString Hex);

/*
* Inline wrapper function for HexToBytes allowing for cleaner usage
*/
uint8* HexToBytesInline(FString in, uint32 size);

//trims the 0x in front used in couple places
FString TrimHex(FString Hex);