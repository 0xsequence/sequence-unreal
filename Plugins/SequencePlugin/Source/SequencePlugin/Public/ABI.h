#pragma once
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

struct FABIArg
{
	EABIArgType Type;
	uint32 Length; // For RAW types, this is the number of bytes. For Arrays, this is the number of entries.
	void* Data;

	uint8 GetBlockNum() const;
	void Encode(uint8* Start, uint8** Head, uint8** Tail);
	void Decode(uint8* Start, uint8** Head, uint8** Tail);
};

class ABI
{
public:
	static FBinaryData Encode(FString Method, FABIArg** Args, uint8 ArgNum);
	static void Decode(FBinaryData Data, FABIArg** Args, uint8 ArgNum);
};

FBinaryData String_to_UTF8(FString String);
FString UTF8_to_String(FBinaryData BinaryData);
void CopyUInt32(uint8* Ptr, uint32 Data);