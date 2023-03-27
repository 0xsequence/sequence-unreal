#pragma once

TOptional<FString> IntToHexString(uint64 Num);
TOptional<FString> IntToHexLetter(uint Num);
TOptional<uint32> HexLetterToInt(FString Hex);
TOptional<uint32> HexStringToInt(FString Hex);