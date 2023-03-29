#pragma once
#include "Types.h"

FString IntToHexString(uint64 Num);
FString IntToHexLetter(uint8 Num);
FString Hash256ToHexString(Hash256 Hash);
TOptional<uint32> HexLetterToInt(FString Hex);
TOptional<uint32> HexStringToInt(FString Hex);
Hash256 HexStringToHash256(FString Hex);