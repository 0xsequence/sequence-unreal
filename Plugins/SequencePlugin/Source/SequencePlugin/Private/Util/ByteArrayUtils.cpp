#include "Util/ByteArrayUtils.h"

FString FByteArrayUtils::BytesToHexString(const TArray<uint8>& Bytes)
{
	FString HexString = TEXT("0x");
	HexString.Reserve(2 + Bytes.Num() * 2);

	for (uint8 Byte : Bytes)
	{
		HexString += FString::Printf(TEXT("%02X"), Byte);
	}

	return HexString;
}

bool FByteArrayUtils::HexStringToBytes(const FString& HexString, TArray<uint8>& OutBytes)
{
	FString CleanHex = HexString;

	if (CleanHex.StartsWith(TEXT("0x")) || CleanHex.StartsWith(TEXT("0X")))
	{
		CleanHex = CleanHex.RightChop(2);
	}

	if (CleanHex.Len() % 2 != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid hex string length"));
		return false;
	}

	OutBytes.Empty(CleanHex.Len() / 2);
	OutBytes.Reserve(CleanHex.Len() / 2);

	for (int32 i = 0; i < CleanHex.Len(); i += 2)
	{
		FString ByteString = CleanHex.Mid(i, 2);
		uint8 Byte = FParse::HexDigit(ByteString[0]) << 4 | FParse::HexDigit(ByteString[1]);

		if (Byte == 0 && !(ByteString.Equals(TEXT("00"))))
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid hex digit in input: %s"), *ByteString);
			return false;
		}

		OutBytes.Add(Byte);
	}

	return true;
}


TArray<uint8> FByteArrayUtils::ConcatBytes(const TArray<TArray<uint8>>& Arrays)
{
	int32 TotalSize = 0;
	for (const auto& Arr : Arrays)
	{
		TotalSize += Arr.Num();
	}

	TArray<uint8> Result;
	Result.Reserve(TotalSize);

	for (const auto& Arr : Arrays)
	{
		Result.Append(Arr);
	}

	return Result;
}

TArray<uint8> FByteArrayUtils::PadLeft(const TArray<uint8>& Data, int32 TotalLength)
{
	if (Data.Num() >= TotalLength)
	{
		return Data;
	}

	int32 PadCount = TotalLength - Data.Num();

	TArray<uint8> Result;
	Result.Reserve(TotalLength);

	Result.AddUninitialized(PadCount);
	FMemory::Memset(Result.GetData(), 0x00, PadCount);

	Result.Append(Data);

	return Result;
}

TArray<uint8> FByteArrayUtils::PadRight(const TArray<uint8>& Data, int32 TotalLength)
{
	if (Data.Num() >= TotalLength)
	{
		return Data;
	}

	TArray<uint8> Result = Data;
	int32 PadCount = TotalLength - Data.Num();

	Result.AddUninitialized(PadCount);
	FMemory::Memset(Result.GetData() + Data.Num(), 0x00, PadCount);

	return Result;
}
