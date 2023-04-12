#pragma once
#include "Types.h"

FString IntToHexString(uint64 Num);
FString IntToHexLetter(uint8 Num);
TOptional<uint8> HexLetterToInt(FString Hex);
TOptional<uint64> HexStringToInt64(FString Hex);

FString HashToHexString(ByteLength Size, uint8* Hash);
uint8* HexStringToHash(ByteLength Size, FString Hex);
FBigNum HexStringToBigNum(FString Hex); // Hex String to Number of arbitrary size

FString Hash256ToHexString(Hash256 Hash);
Hash256 HexStringToHash256(FString Hex);

FString AddressToHexString(Address Addr);
Address HexStringToAddress(FString Hex);