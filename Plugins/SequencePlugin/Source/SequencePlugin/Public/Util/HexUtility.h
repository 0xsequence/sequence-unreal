// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "CoreMinimal.h"
#include "Containers/StaticArray.h"
#include "GenericPlatform/GenericPlatform.h"
#include "Types/Types.h"

static constexpr int32 HexLUTSize = 16;
static const FString HexLUTNew[] = {"0","1","2","3","4","5","6","7","8","9","a","b","c","d","e","f"};

/*
 * Converts a uint64 to a hex FString
 */
FString SEQUENCEPLUGIN_API IntToHexString(uint64 Num);

/*
 * Converts a single Byte to a hex pair
 */
FString SEQUENCEPLUGIN_API IntToHexLetter(uint8 Num);

/*
 * Converts a hex string to a uint64
 */
TOptional<uint64> SEQUENCEPLUGIN_API HexStringToUint64(FString Hex);

uint8_t SEQUENCEPLUGIN_API GetPaddingByte(int32 StringLength);

TArray<uint8_t> SEQUENCEPLUGIN_API PKCS7(FString in);

/*
* Generates a random hex character
*/
FString SEQUENCEPLUGIN_API RandomHexCharacter();

/*
* Generates a random hex byte
*/
FString SEQUENCEPLUGIN_API RandomHexByte();

/*
* Generates a random Byte
*/
uint8 SEQUENCEPLUGIN_API RandomByte();

/*
* Inline wrapper function for HexToBytes allowing for cleaner usage
*/
TArray<uint8> SEQUENCEPLUGIN_API HexToBytesInline(FString in);

template<ByteLength TSize>
TStaticArray<uint8, TSize> SEQUENCEPLUGIN_API HexToBytesInline(FString in)
{
	TStaticArray<uint8, TSize> Arr;
	HexToBytes(in, Arr.GetData());
	return Arr;
}

//trims the 0x in front used in couple places
FString SEQUENCEPLUGIN_API TrimHex(FString Hex);

//Makes
TArray<uint8> SEQUENCEPLUGIN_API MakeArray(uint8* Ptr, int Len);

template<ByteLength TSize>
TStaticArray<uint8, TSize> SEQUENCEPLUGIN_API MakeArray(uint8* Ptr)
{
	TStaticArray<uint8, TSize> Arr;
	for(int i = 0; i < TSize; i++) Arr[i] = Ptr[i];
	return Arr;
}