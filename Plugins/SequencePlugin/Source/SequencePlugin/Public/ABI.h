#pragma once
#include "BinaryData.h"
#include "Types.h"

inline uint8 GMethodIdByteLength = 4;
inline uint8 GBlockByteLength = 32;

enum EABIArgType
{
	STATIC, // Integers, bytes, and hashes
	BYTES,
	STRING,
	ARRAY,
};

FString TypeToString(EABIArgType Type);

struct FABIArg
{
	EABIArgType Type;
	uint32 Length; // For RAW types, this is the number of bytes. For Arrays, this is the number of entries.
	void* Data;

	uint8 GetBlockNum() const;
	void Encode(uint8* Start, uint8** Head, uint8** Tail);
	FABIArg Decode(uint8* TrueStart, uint8* Start, uint8* Head);
	FNonUniformData ToBinary() const;
	FString ToHex() const;
	FString ToString() const;
	uint8 ToUint8() const;
	FABIArg* ToArr() const;
	void Log();

	// Free Data
	void Destroy();

	static FABIArg Empty(EABIArgType Type); // initializes it without data

	// Creates data; must be destroyed
	static FABIArg New(FString Value);
	static FABIArg New(uint8 Value);
	static FABIArg New(uint32 Value);
	static FABIArg New(FABIArg* Array, uint32 Length); 
};

class ABI
{
public:
	static FNonUniformData Encode(FString Method, FABIArg* Args, uint8 ArgNum);
	static void Decode(FNonUniformData Data, FABIArg* Args, uint8 ArgNum);
};

void CopyInUint32(uint8* BlockPtr, uint32 Value);
uint32 CopyOutUint32(uint8* BlockPtr);