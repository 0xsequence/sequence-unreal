#pragma once
#include "Types.h"

FString IntToHexString(uint64 Num);
FString IntToHexLetter(uint8 Num);
TOptional<uint8> HexLetterToInt(FString Hex);
TOptional<uint64> HexStringToInt64(FString Hex);

FString HashToHexString(ByteLength Size, Hash Hash);
Hash HexStringToHash(ByteLength Size, FString Hex);


FString TrimHex(FString Hex);