#pragma once

class SEQUENCEPLUGIN_API FByteArrayUtils
{
public:
	static FString BytesToHexString(const TArray<uint8>& Bytes);

	static FString BytesToBigIntHexString(const TArray<uint8>& Bytes);
	
	static TArray<uint8> HexStringToBytes(const FString& HexString);

	static TArray<uint8> ByteArrayFromNumber(const int32 Value, const int32 Size);

	static TArray<uint8> ConcatBytes(const TArray<TArray<uint8>>& Arrays);

	static TArray<uint8> PadLeft(const TArray<uint8>& Data, int32 TotalLength);
	
	static TArray<uint8> PadRight(const TArray<uint8>& Data, int32 TotalLength);
};
