#pragma once
#include "Types.h"

uint8* BlankArray(ByteLength size);

struct FBinaryData
{
	virtual ~FBinaryData() = default; // Does NOT free Data pointer. Must do so manually!
	uint8* Arr;
	virtual const ByteLength GetLength() = 0;
	void Destroy();
	FString ToHex();
	void Renew(); // Makes new blank array. Clean up the old one!
};

// Number of arbitrary length
// Must be deallocated
struct FNonUniformData : FBinaryData
{
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

struct FUniformData : FBinaryData
{
	FNonUniformData Copy(); // This creates new data that must be freed
}; // Data with set sizes

// Basic Binary Types
struct FHash256 : FUniformData
{
	static constexpr ByteLength Size = 32;
	static FHash256 New(); // This creates new data that must be freed
	static FHash256 From(uint8* Arr);
	static FHash256 From(FString Str);
	virtual const ByteLength GetLength() override;
};

struct FAddress : FUniformData
{
	static constexpr ByteLength Size = 20;
	static FAddress New(); // This creates new data that must be freed
	static FAddress From(uint8* Arr);
	static FAddress From(FString Str);
	virtual const ByteLength GetLength() override;
};

struct FPublicKey : FUniformData
{
	static constexpr ByteLength Size = 64;
	static FPublicKey New(); // This creates new data that must be freed
	static FPublicKey From(uint8* Arr);
	static FPublicKey From(FString Str);
	virtual const ByteLength GetLength() override;
};

struct FPrivateKey : FUniformData
{
	static constexpr ByteLength Size = 32;
	static FPrivateKey New(); // This creates new data that must be freed
	static FPrivateKey From(uint8* Arr);
	static FPrivateKey From(FString Str);
	virtual const ByteLength GetLength() override;
};

struct FBloom : FUniformData
{
	static constexpr ByteLength Size = 256;
	static FBloom New(); // This creates new data that must be freed
	static FBloom From(uint8* Arr);
	static FBloom From(FString Str);
	virtual const ByteLength GetLength() override;
};

struct FBlockNonce : FUniformData
{
	static constexpr ByteLength Size = 8;
	static FBlockNonce New(); // This creates new data that must be freed
	static FBlockNonce From(uint8* Arr);
	static FBlockNonce From(FString Str);
	virtual const ByteLength GetLength() override;
	void Increment();
};

