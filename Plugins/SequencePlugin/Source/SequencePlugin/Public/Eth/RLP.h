// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Types/BinaryData.h"
#include "Types/Types.h"

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
RLPItem Itemize(const FBinaryData &Data);
RLPItem Itemize(FUnsizedData Data);
RLPItem Itemize(Hash Hash, ByteLength Length);
RLPItem Itemize(RLPItem* Items, uint32 Length);

class RLP
{
public:
	static FUnsizedData Encode(RLPItem Item);
};
