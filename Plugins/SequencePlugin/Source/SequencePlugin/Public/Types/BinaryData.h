#pragma once
#include "Types.h"

TArray<uint8> BlankArray(ByteLength Size);

struct FBinaryData
{
	virtual ~FBinaryData() = default; // Does NOT free Data pointer. Must do so manually!
	TSharedPtr<TArray<uint8>> Arr;
	virtual ByteLength GetLength() const = 0;
	FString ToHex() const;
	void Renew(); // Makes new blank array. Clean up the old one!
	uint8* Ptr() const;
	TArray<uint8> ToArray();
};

// Number of arbitrary length
struct FUnsizedData final : FBinaryData
{
	static FUnsizedData Empty();
	FUnsizedData(const TArray<uint8> &Array);
	FUnsizedData Copy() const; // This creates new data that must be freed
	virtual ByteLength GetLength() const override;
	FUnsizedData Trim(); // Destroys itself
	static FUnsizedData From(FString Hex);
};

// Returns returns data with leading 0x00 bytes trimmed
FUnsizedData Trimmed(const FBinaryData &Data);

FUnsizedData HexStringToBinary(FString Hex); // Hex String to Number of arbitrary size
FUnsizedData StringToUTF8(FString String);
FString UTF8ToString(FUnsizedData BinaryData);

// UNIFORM DATA TYPES

template<ByteLength TSize>
struct TSizedData : FBinaryData
{
	const static ByteLength Size = TSize;
	virtual ByteLength GetLength() const override;
	FUnsizedData Copy() const; // This creates new data that must be freed
	explicit operator FUnsizedData() const { return Copy(); }
	static TSizedData Empty();
}; // Data with set sizes

template <ByteLength Size>
inline ByteLength TSizedData<Size>::GetLength() const 
{
	return Size;
}

template <ByteLength Size>
FUnsizedData TSizedData<Size>::Copy() const
{
	FUnsizedData data = FUnsizedData::Empty();
	data.Arr.Get()->Append(this->Arr);
	return data;
}

template <ByteLength TSize>
TSizedData<TSize> TSizedData<TSize>::Empty()
{
	return TSizedData{};
}

// Basic Binary Types
struct FHash256 final : TSizedData<32>
{
	static FHash256 New(); // This creates new data that must be freed
	static FHash256 From(TStaticArray<uint8, 32> &Arr);
	static FHash256 From(TArray<uint8> &Arr);
	static FHash256 From(FString Str);
};

struct FAddress final : TSizedData<20>
{
	static FAddress New(); // This creates new data that must be freed
	static FAddress From(TStaticArray<uint8, 20> &Arr);
	static FAddress From(FString Str);
};

struct FPublicKey final : TSizedData<64>
{
	static FPublicKey New(); // This creates new data that must be freed
	static FPublicKey From(TStaticArray<uint8, 64> &Arr);
	static FPublicKey From(FString Str);
};

struct FPrivateKey final : TSizedData<32>
{
	static FPrivateKey New(); // This creates new data that must be freed
	static FPrivateKey From(TStaticArray<uint8, 32> &Arr);
	static FPrivateKey From(FString Str);
};

struct FBloom final : TSizedData<256>
{
	static FBloom New(); // This creates new data that must be freed
	static FBloom From(TStaticArray<uint8, 256> &Arr);
	static FBloom From(FString Str);
};

struct FBlockNonce final : TSizedData<8>
{
	static FBlockNonce New(); // This creates new data that must be freed
	static FBlockNonce From(TStaticArray<uint8, 8> &Arr);
	static FBlockNonce From(FString Str);
	void Increment() const;
};

