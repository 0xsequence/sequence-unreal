#include "Util/ByteArrayUtils.h"

TArray<uint8> FByteArrayUtils::StringToBytes(const FString& InString)
{
	FTCHARToUTF8 Converter(*InString);
	TArray<uint8> Bytes;
	Bytes.Append((uint8*)Converter.Get(), Converter.Length());
	return Bytes;
}

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

FString FByteArrayUtils::BytesToBigIntHexString(const TArray<uint8>& Bytes)
{
	FString Out;
	for (uint8 Byte : Bytes)
	{
		Out += FString::Printf(TEXT("%02x"), Byte);
	}

	// Strip leading zeros
	int32 FirstNonZero = 0;
	while (FirstNonZero < Out.Len() && Out[FirstNonZero] == '0')
		++FirstNonZero;

	FString Trimmed = (FirstNonZero < Out.Len()) ? Out.Mid(FirstNonZero) : TEXT("0");

	return TEXT("0x") + Trimmed;
}

TArray<uint8> FByteArrayUtils::HexStringToBytes(const FString& HexString)
{
	FString CleanHex = HexString;

	if (CleanHex.StartsWith(TEXT("0x")) || CleanHex.StartsWith(TEXT("0X")))
	{
		CleanHex = CleanHex.RightChop(2);
	}

	if (CleanHex.Len() % 2 != 0)
	{
		CleanHex = FString::Printf(TEXT("0%s"), *CleanHex);
	}

	TArray<uint8> OutBytes;
	OutBytes.Empty(CleanHex.Len() / 2);
	OutBytes.Reserve(CleanHex.Len() / 2);

	for (int32 i = 0; i < CleanHex.Len(); i += 2)
	{
		FString ByteString = CleanHex.Mid(i, 2);
		uint8 Byte = FParse::HexDigit(ByteString[0]) << 4 | FParse::HexDigit(ByteString[1]);

		if (Byte == 0 && !(ByteString.Equals(TEXT("00"))))
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid hex digit in input: %s"), *ByteString);
			return TArray<uint8>();
		}

		OutBytes.Add(Byte);
	}

	return OutBytes;
}

TArray<uint8> FByteArrayUtils::ByteArrayFromNumber(const int32 Value, const int32 Size)
{
	if (Size < 1 || Size > 4)
	{
		UE_LOG(LogTemp, Error, TEXT("Size must be between 1 and 4 bytes for an int."));
		return {};
	}

	uint8 Bytes[4];
	Bytes[0] = static_cast<uint8>((Value >> 24) & 0xFF);
	Bytes[1] = static_cast<uint8>((Value >> 16) & 0xFF);
	Bytes[2] = static_cast<uint8>((Value >> 8) & 0xFF);
	Bytes[3] = static_cast<uint8>(Value & 0xFF);

	int32 StartIndex = 4 - Size;

	TArray<uint8> Out;
	Out.Append(Bytes + StartIndex, Size);
	return Out;
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

uint32 FByteArrayUtils::MinBytesFor(const uint64& Value)
{
	if (Value <= 0xFFULL)
		return 1;
	if (Value <= 0xFFFFULL)
		return 2;
	if (Value <= 0xFFFFFFULL)
		return 3;
	if (Value <= 0xFFFFFFFFULL)
		return 4;
	if (Value <= 0xFFFFFFFFFFULL)
		return 5;
	if (Value <= 0xFFFFFFFFFFFFULL)
		return 6;
	if (Value <= 0xFFFFFFFFFFFFFFULL)
		return 7;
	
	return 8;
}

FString FByteArrayUtils::BytesToHexString(const TArray<uint8>& Bytes, int32 Start, int32 Size)
{
	FString Out;
	for (int32 i = 0; i < Size; ++i)
		Out += FString::Printf(TEXT("%02x"), Bytes[Start + i]);
	return TEXT("0x") + Out;
}

int32 FByteArrayUtils::BytesToInt(const TArray<uint8>& Bytes, int32 Start, int32 Size)
{
	int32 Value = 0;
	for (int32 i = 0; i < Size; ++i)
	{
		Value = (Value << 8) | Bytes[Start + i];
	}
	return Value;
}

void FByteArrayUtils::CopyBytes(const TArray<uint8>& Src, int32 Start, int32 Size, TArray<uint8>& Dest)
{
	Dest.SetNum(Size);
	FMemory::Memcpy(Dest.GetData(), Src.GetData() + Start, Size);
}

TArray<uint8> FByteArrayUtils::SliceBytes(const TArray<uint8>& Src, int32 Start, int32 Size)
{
	TArray<uint8> Out;
	Out.Append(Src.GetData() + Start, Size);
	return Out;
}

TArray<uint8> FByteArrayUtils::SliceBytesFrom(const TArray<uint8>& Bytes, int32 Start)
{
	TArray<uint8> Out;

	if (Start < 0 || Start >= Bytes.Num())
	{
		return Out;
	}

	const int32 Size = Bytes.Num() - Start;
	Out.Append(Bytes.GetData() + Start, Size);
	return Out;
}
