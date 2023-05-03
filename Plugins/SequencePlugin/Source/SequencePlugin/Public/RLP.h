#pragma once
#include "Types.h"

ByteLength GetByteLength(uint32 Length);

enum RLPItemType
{
	BINARY,
	LIST
};

struct RLPItem
{
	RLPItemType Type;
	uint32 Length;
	uint32 LengthSum;
	void* Data;

	ByteLength CalculateLength();
	void Encode(uint8* HeadPtr);
};

RLPItem Itemize(FString String);
RLPItem Itemize(FBinaryData Data);
RLPItem Itemize(Hash Hash, ByteLength Length);
RLPItem Itemize(RLPItem* Items, uint32 Length);

class RLP
{
public:
	static FBinaryData Encode(RLPItem Item);
};
