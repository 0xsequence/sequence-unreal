#pragma once
#include "Types.h"

uint8* BlankArray(ByteLength size);

struct FBinaryData
{
	virtual ~FBinaryData() = default; // Does NOT free Data pointer. Must do so manually!
	uint8* Arr;
	virtual const ByteLength GetLength() = 0;
	void Destroy();
	const FString ToHex();
	void Renew(); // Makes new blank array. Clean up the old one!
};

// Number of arbitrary length
// Must be deallocated
struct FNonUniformData : FBinaryData
{
	static FNonUniformData Empty();
	ByteLength Length;
	FNonUniformData(uint8* Arr, ByteLength Length);
	FNonUniformData Copy(); // This creates new data that must be freed
	virtual const ByteLength GetLength() override;
	FNonUniformData Trim(); // Destroys itself
};

// Returns returns data with leading 0x00 bytes trimmed
FNonUniformData Trimmed(FBinaryData &Data);

FNonUniformData HexStringToBinary(FString Hex); // Hex String to Number of arbitrary size
FNonUniformData StringToUTF8(FString String);
FString UTF8ToString(FNonUniformData BinaryData);

// UNIFORM DATA TYPES

template<ByteLength TSize>
struct FUniformData : FBinaryData
{
	const static ByteLength Size = TSize;
	virtual const ByteLength GetLength() override;
	FNonUniformData Copy(); // This creates new data that must be freed
	operator FNonUniformData() { return Copy(); }
}; // Data with set sizes

template <ByteLength Size>
const inline ByteLength FUniformData<Size>::GetLength()
{
	return Size;
}

template <ByteLength Size>
FNonUniformData FUniformData<Size>::Copy()
{
	auto Length = GetLength();
	auto NewArr = new uint8[Length];
	for(auto i = 0; i < Length; i++) NewArr[i] = Arr[i];
	return FNonUniformData{NewArr, Length};
}

// Basic Binary Types
struct FHash256 : FUniformData<32>
{
	static FHash256 New(); // This creates new data that must be freed
	static FHash256 From(uint8* Arr);
	static FHash256 From(FString Str);
};

struct FAddress : FUniformData<20>
{
	static FAddress New(); // This creates new data that must be freed
	static FAddress From(uint8* Arr);
	static FAddress From(FString Str);
};

struct FPublicKey : FUniformData<64>
{
	static FPublicKey New(); // This creates new data that must be freed
	static FPublicKey From(uint8* Arr);
	static FPublicKey From(FString Str);
};

struct FPrivateKey : FUniformData<32>
{
	static FPrivateKey New(); // This creates new data that must be freed
	static FPrivateKey From(uint8* Arr);
	static FPrivateKey From(FString Str);
};

struct FBloom : FUniformData<256>
{
	static FBloom New(); // This creates new data that must be freed
	static FBloom From(uint8* Arr);
	static FBloom From(FString Str);
};

struct FBlockNonce : FUniformData<8>
{
	static FBlockNonce New(); // This creates new data that must be freed
	static FBlockNonce From(uint8* Arr);
	static FBlockNonce From(FString Str);
	void Increment();
};

