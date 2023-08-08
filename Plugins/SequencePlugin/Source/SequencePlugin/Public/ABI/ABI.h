#pragma once
#include "Types/BinaryData.h"
#include "Types/Types.h"
#include "ABI/ABITypes.h"

inline uint8 GMethodIdByteLength = 4;
inline uint8 GBlockByteLength = 32;
FUnsizedData NewEmptyBlock();

enum EABIArgType
{
	STATIC, // Integers, bytes, and hashes
	BYTES, 
	STRING,
	ARRAY,
	FIXED // Like array, but no size attribute
	//Address,
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
	FUnsizedData ToBinary() const;
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
	static void ParseMethod(FString& Method);
	static FUnsizedData Encode(FString Method, TArray<FABIProperty*> &Args);
	static FUnsizedData EncodeArgs(FString Method, FABIArg* Args, uint8 ArgNum);
	static FUnsizedData EncodeArgsWithoutSignature(FABIArg* Args, uint8 ArgNum);
	static void DecodeArgs(FUnsizedData Data, FABIArg* Args, uint8 ArgNum);
	static void DecodeArgsWithoutSignature(FUnsizedData Data, FABIArg* Args, uint8 ArgNum);
	static void Decode(FUnsizedData Data, TArray<FABIProperty*> &Args);
};

void CopyInUint32(uint8* BlockPtr, uint32 Value);
uint32 CopyOutUint32(uint8* BlockPtr);