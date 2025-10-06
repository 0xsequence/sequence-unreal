#pragma once

class SEQUENCEPLUGIN_API FByteArrayUtils
{
public:
	static FString BytesToHexString(const TArray<uint8>& Bytes);
	
	static bool HexStringToBytes(const FString& HexString, TArray<uint8>& OutBytes);

	static TArray<uint8> ConcatBytes(const TArray<TArray<uint8>>& Arrays);

	static TArray<uint8> PadLeft(const TArray<uint8>& Data, int32 TotalLength);
	
	static TArray<uint8> PadRight(const TArray<uint8>& Data, int32 TotalLength);
};
