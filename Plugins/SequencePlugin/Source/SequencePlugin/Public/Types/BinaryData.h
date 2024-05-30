// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Types.h"
#include "Containers/StaticArray.h"

TArray<uint8> BlankArray(ByteLength Size);

// Struct encapsulating an array of binary data
struct SEQUENCEPLUGIN_API FBinaryData
{
	virtual ~FBinaryData() = default;

	// Raw data- direct modification is usually inadvisable
	TSharedPtr<TArray<uint8>> Arr;

	// Byte length
	virtual ByteLength GetLength() const = 0;
	
	// String hex representation of the binary data
	FString ToHex() const; 

	// Clear data, replacing it with a new blank array
	void Renew();

	// Direct byte access
	uint8* Ptr() const;

	// Get byte array
	TArray<uint8> ToArray();
};

// Binary data of arbitrary length
struct SEQUENCEPLUGIN_API FUnsizedData final : FBinaryData
{
	static FUnsizedData Empty();
	FUnsizedData(const TArray<uint8> &Array);
	FUnsizedData Copy() const;
	virtual ByteLength GetLength() const override;
	FUnsizedData Trim();
	static FUnsizedData From(FString Hex);
};

// Returns returns data with leading 0x00 bytes trimmed
FUnsizedData SEQUENCEPLUGIN_API Trimmed(const FBinaryData &Data);

// Hex String to Number of arbitrary size
FUnsizedData SEQUENCEPLUGIN_API HexStringToBinary(FString Hex);

// Gets UTF8 byte encoding of string
FUnsizedData SEQUENCEPLUGIN_API StringToUTF8(FString String);

// Converts a UTF8 encoded byte array ot a string
FString SEQUENCEPLUGIN_API UTF8ToString(FUnsizedData BinaryData);

// UNIFORM DATA TYPES
template<ByteLength TSize>
struct SEQUENCEPLUGIN_API TSizedData : FBinaryData
{
	const static ByteLength Size = TSize;
	
	virtual ByteLength GetLength() const override
	{
		return Size;
	}
	
	FUnsizedData Copy() const
	{
		FUnsizedData Data = FUnsizedData::Empty();
		Data.Arr.Get()->Append(*this->Arr.Get());
		return Data;
	}
	
	explicit operator FUnsizedData() const { return Copy(); }
	
	template <ByteLength Size>
	static TSizedData<Size> Empty()
	{
		return TSizedData{};
	}
}; // Data with set sizes

// Basic Binary Types
struct SEQUENCEPLUGIN_API FHash256 final : TSizedData<32>
{
	static FHash256 New();
	static FHash256 From(TStaticArray<uint8, 32> &Arr);
	static FHash256 From(TArray<uint8> &Arr);
	static FHash256 From(FString Str);
};

struct SEQUENCEPLUGIN_API FAddress final : TSizedData<20>
{
	static FAddress New();
	static FAddress From(TStaticArray<uint8, 20> &Arr);
	static FAddress From(FString Str);
};

struct SEQUENCEPLUGIN_API FPublicKey final : TSizedData<64>
{
	static FPublicKey New();
	static FPublicKey From(TStaticArray<uint8, 64> &Arr);
	static FPublicKey From(FString Str);
};

struct SEQUENCEPLUGIN_API FPrivateKey final : TSizedData<32>
{
	static FPrivateKey New();
	static FPrivateKey From(TStaticArray<uint8, 32> &Arr);
	static FPrivateKey From(FString Str);
};

struct SEQUENCEPLUGIN_API FBloom final : TSizedData<256>
{
	static FBloom New();
	static FBloom From(TStaticArray<uint8, 256> &Arr);
	static FBloom From(FString Str);
};

struct SEQUENCEPLUGIN_API FBlockNonce final : TSizedData<8>
{
	static FBlockNonce New();
	static FBlockNonce From(TStaticArray<uint8, 8> &Arr);
	static FBlockNonce From(FString Str);
	void Increment() const;
};