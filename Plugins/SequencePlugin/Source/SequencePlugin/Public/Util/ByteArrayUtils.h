#pragma once

class SEQUENCEPLUGIN_API FByteArrayUtils
{
public:
	static TArray<uint8> StringToBytes(const FString& InString);
	
	static FString BytesToHexString(const TArray<uint8>& Bytes);

	static FString BytesToBigIntHexString(const TArray<uint8>& Bytes);
	
	static TArray<uint8> HexStringToBytes(const FString& HexString);

	static TArray<uint8> ByteArrayFromNumber(const int32 Value, const int32 Size);

	static TArray<uint8> ConcatBytes(const TArray<TArray<uint8>>& Arrays);

	static TArray<uint8> PadLeft(const TArray<uint8>& Data, int32 TotalLength);
	
	static TArray<uint8> PadRight(const TArray<uint8>& Data, int32 TotalLength);

	static uint32 MinBytesFor(const uint64& Value);

	static FString BytesToHexString(const TArray<uint8>& Bytes, int32 Start, int32 Size);
	
	static int32 BytesToInt(const TArray<uint8>& Bytes, int32 Start, int32 Size);
	
	static void CopyBytes(const TArray<uint8>& Src, int32 Start, int32 Size, TArray<uint8>& Dest);
	
	static TArray<uint8> SliceBytes(const TArray<uint8>& Src, int32 Start, int32 Size);

	static TArray<uint8> SliceBytesFrom(const TArray<uint8>& Bytes, int32 Start);
};
