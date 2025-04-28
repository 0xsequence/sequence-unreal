// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Types/BinaryData.h"
#include "VectorTypes.h"
#include "Util/HexUtility.h"

TArray<uint8> BlankArray(ByteLength Size)
{
	TArray<uint8> Arr;

	for(uint32 i = 0; i < Size; i++)
	{
		Arr.Push(0x00);
	}

	return Arr;
}

FString FBinaryData::ToHex() const
{
	if(Arr == nullptr)
	{
		return "";
	}
	
	return "0x" + BytesToHex(Ptr(),GetLength()).ToLower();
}

void FBinaryData::Renew()
{
	this->Arr.Reset();
	this->Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	this->Arr.Get()->Append(BlankArray(this->GetLength()));
}

uint8* FBinaryData::Ptr() const
{
	return this->Arr.Get()->GetData();
}

TArray<uint8> FBinaryData::ToArray()
{
	return *this->Arr.Get();
}

FUnsizedData FUnsizedData::Empty()
{
	return FUnsizedData{TArray<uint8>()};
}

FUnsizedData::FUnsizedData(const TArray<uint8> &Array)
{
	this->Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	this->Arr->Append(Array);
}

FUnsizedData FUnsizedData::Copy() const
{
	return FUnsizedData{* this->Arr.Get()};
}

ByteLength FUnsizedData::GetLength() const
{
	return this->Arr->Num();
}

FUnsizedData FUnsizedData::Trim()
{
	FUnsizedData trimmed = Trimmed(*this);
	return trimmed;
}

FUnsizedData FUnsizedData::From(const FString Hex)
{
	return HexStringToBinary(Hex);
}


FUnsizedData Trimmed(const FBinaryData& Data)
{
	TArray<uint8> NewArray = *Data.Arr.Get();

	while(NewArray.Num() > 0 && NewArray[0] == 0x00)
	{
		NewArray.RemoveAt(0);
	}

	return FUnsizedData{ NewArray };
}

FHash256 FHash256::New()
{
	FHash256 Data = FHash256{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Renew();
	return Data;
}

FHash256 FHash256::From(TStaticArray<uint8, Size> &Arr)
{
	FHash256 Data = FHash256{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Arr.Get()->Append(Arr);
	return Data;
}

FHash256 FHash256::From(TArray<uint8>& Arr)
{
	FHash256 Data = FHash256{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Arr->Append(Arr);
	return Data;
}

FHash256 FHash256::From(const FString Str)
{
	auto Arr = HexToBytesInline<Size>(Str);
	return From(Arr);
}

FAddress FAddress::New()
{
	FAddress Data = FAddress{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Renew();
	return Data;
}

FAddress FAddress::From(TStaticArray<uint8, Size>& Arr)
{
	FAddress Data = FAddress{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Arr.Get()->Append(Arr);
	return Data;
}

FAddress FAddress::From(FString Str)
{
	auto Arr = HexToBytesInline<Size>(Str);
	return From(Arr);
}

FPublicKey FPublicKey::New()
{
	FPublicKey Data = FPublicKey{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Renew();
	return Data;
}

FPublicKey FPublicKey::From(TStaticArray<uint8, Size>& Arr)
{
	FPublicKey Data = FPublicKey{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Arr.Get()->Append(Arr);
	return Data;
}

FPublicKey FPublicKey::From(FString Str)
{
	auto Arr = HexToBytesInline<Size>(Str);
	return From(Arr);
}

FPrivateKey FPrivateKey::New()
{
	FPrivateKey Data = FPrivateKey{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Renew();
	return Data;
}

FPrivateKey FPrivateKey::From(TStaticArray<uint8, Size>& Arr)
{
	FPrivateKey Data = FPrivateKey{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Arr.Get()->Append(Arr);
	return Data;
}

FPrivateKey FPrivateKey::From(FString Str)
{
	auto Arr = HexToBytesInline<Size>(Str);
	return From(Arr);
}

FBloom FBloom::New()
{
	FBloom Data = FBloom{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Renew();
	return Data;
}

FBloom FBloom::From(TStaticArray<uint8, 256>& Arr)
{
	FBloom Data = FBloom{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Arr.Get()->Append(Arr);
	return Data;
}

FBloom FBloom::From(FString Str)
{
	auto Arr = HexToBytesInline<Size>(Str);
	return From(Arr);
}

FBlockNonce FBlockNonce::New()
{
	FBlockNonce Data = FBlockNonce{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Renew();
	return Data;
}

FBlockNonce FBlockNonce::From(TStaticArray<uint8, 8>& Arr)
{
	FBlockNonce Data = FBlockNonce{};
	Data.Arr = MakeShared<TArray<uint8>>(TArray<uint8>());
	Data.Arr.Get()->Append(Arr);
	return Data;
}

FBlockNonce FBlockNonce::From(const FString Str)
{
	auto Arr = HexToBytesInline<Size>(Str);
	return From(Arr);
}

void FBlockNonce::Increment() const
{
	uint8* ArrayPtr = Arr.Get()->GetData();
	for(uint32 i = 0; i < GetLength(); i--)
	{
		const int Index = GetLength() - 1 - i;
		ArrayPtr[Index] += 1;

		if(ArrayPtr[Index] != 0x00)
		{
			return;
		}
	}
}

FUnsizedData StringToUTF8(FString String)
{
	const uint32 Length = String.Len();

	uint8* Binary = new uint8[Length];

	StringToBytes(String, Binary, Length);

	// I have no idea why I need to add 1 but it works
	for(uint32 i = 0; i < Length; i++)
	{
		Binary[i] = Binary[i] + 1;
	}

	FUnsizedData Data = FUnsizedData{MakeArray(Binary, Length)};
	delete [] Binary;
	
	return Data;
}

FString UTF8ToString(FUnsizedData BinaryData)
{
	TArray<uint8> Buffer;
	for(uint32 i = 0; i < BinaryData.GetLength(); i++)
	{
		Buffer.Add(BinaryData.Arr.Get()->GetData()[i]);
	}
	Buffer.Add('\0');
	return reinterpret_cast<const char*>(Buffer.GetData());
}

FUnsizedData HexStringToBinary(const FString Hex)
{
	FString HexCopy = FString(Hex);
	return FUnsizedData(HexToBytesInline(Hex));
}