#pragma once
#include "Types.h"

FString IntToHexString(uint64 Num);
FString IntToHexLetter(uint8 Num);
TOptional<uint32> HexLetterToInt(FString Hex);
TOptional<uint32> HexStringToInt(FString Hex);

FString HashToHexString(uint8 Size, uint8* Hash);
uint8* HexStringToHash(uint8 Size, FString Hex);

FString Hash256ToHexString(Hash256 Hash);
Hash256 HexStringToHash256(FString Hex);

FString AddressToHexString(Address Addr);
Address HexStringToAddress(FString Hex);