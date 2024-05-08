// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "ABI/ABIEncodeable.h"
#include "ABI/ABI.h"

void ABIEncodeable::PushEmptyBlock(TArray<uint8>& Data)
{
	for(int i = 0; i < GBlockByteLength; i++)
	{
		Data.Push(0x00);
	}
}

void ABIEncodeable::CopyInUInt32(TArray<uint8>& Data, uint32 Value, int BlockPosition)
{
	for(int i = 0; i < 4; i++)
	{
		Data[BlockPosition + GBlockByteLength - 1 - i] = Value >> 8 * i;
	}
}

void ABIEncodeable::CopyInInt32(TArray<uint8>& Data, int32 Value, int BlockPosition)
{
	for(int i = 0; i < 4; i++)
	{
		Data[BlockPosition + GBlockByteLength - 1 - i] = Value >> 8 * i;
	}
}

TFixedABIArray::TFixedABIArray() : MyData(TArray<ABIEncodeable*>())
{
}

TFixedABIArray::TFixedABIArray(TArray<ABIEncodeable*> MyData) : MyData(MyData)
{
}

TArray<uint8> TFixedABIArray::Encode()
{
	TArray<uint8> Data;
	int32 HeadOffset = Data.Num();

	for(auto i = 0; i < MyData.Num(); i++)
	{
		MyData[i]->EncodeHead(Data);
	}

	for(auto i = 0; i < MyData.Num(); i++)
	{
		MyData[i]->EncodeTail(Data, HeadOffset, i);
	}
	
	return Data;
}

void TFixedABIArray::EncodeHead(TArray<uint8>& Data)
{
	Data.Append(Encode());
}

void TFixedABIArray::EncodeTail(TArray<uint8>& Data, int HeadPosition, int Offset)
{
	// Fixed data does not have a tail
}

void TFixedABIArray::Push(ABIEncodeable* Arg)
{
	MyData.Push(Arg);
}

TDynamicABIArray::TDynamicABIArray() : MyData(TArray<ABIEncodeable*>())
{
}

TDynamicABIArray::TDynamicABIArray(TArray<ABIEncodeable*> MyData) : MyData(MyData)
{
}

void TDynamicABIArray::EncodeHead(TArray<uint8>& Data)
{
	// Head will be encoded later
	PushEmptyBlock(Data);
}

void TDynamicABIArray::EncodeTail(TArray<uint8>& Data, int HeadPosition, int Offset)
{
	// Encode Head
	uint32 DistanceToHead = Data.Num() - HeadPosition;
	CopyInUInt32(Data, DistanceToHead, HeadPosition + Offset * GBlockByteLength);

	// Encode Length
	int32 BlockPos = Data.Num();
	PushEmptyBlock(Data);
	CopyInUInt32(Data, MyData.Num(), BlockPos);

	// Encode Data
	int HeadOffset = Data.Num();

	for(auto i = 0; i < MyData.Num(); i++)
	{
		MyData[i]->EncodeHead(Data);
	}

	for(auto i = 0; i < MyData.Num(); i++)
	{
		MyData[i]->EncodeTail(Data, HeadOffset, i);
	}
}

void TDynamicABIArray::Push(ABIEncodeable* Arg)
{
	MyData.Push(Arg);
}

TFixedABIData::TFixedABIData(TArray<uint8> MyData) : MyData(MyData)
{
	
}

void TFixedABIData::EncodeHead(TArray<uint8>& Data)
{
	uint32 BlockLen = (MyData.Num() + GBlockByteLength - 1) / GBlockByteLength;
	
	Data.Append(&MyData[0], MyData.Num());

	uint32 ZeroesToAdd = BlockLen * GBlockByteLength - MyData.Num();
	for(uint32 i = 0; i < ZeroesToAdd; i++)
	{
		Data.Push(0x00);
	}
}

void TFixedABIData::EncodeTail(TArray<uint8>& Data, int HeadPosition, int Offset)
{
	// Fixed Data has no Tail
}

TDynamicABIData::TDynamicABIData(TArray<uint8> MyData) : MyData(MyData)
{
}

void TDynamicABIData::EncodeHead(TArray<uint8> &Data)
{
	// Head will be encoded later
	PushEmptyBlock(Data);
}

void TDynamicABIData::EncodeTail(TArray<uint8> &Data, int HeadPosition, int Offset)
{
	// Encode Head
	uint32 DistanceToHead = Data.Num() - HeadPosition;
	CopyInUInt32(Data, DistanceToHead, HeadPosition + Offset * GBlockByteLength);
	
	// Encode Length
	int32 BlockPos = Data.Num();
	PushEmptyBlock(Data);
	CopyInUInt32(Data, MyData.Num(), BlockPos);

	// Encode Data
	uint32 BlockLen = (MyData.Num() + GBlockByteLength - 1) / GBlockByteLength;
	Data.Append(&MyData[0], MyData.Num());
	uint32 ZeroesToAdd = BlockLen * GBlockByteLength - MyData.Num();
	for(uint32 i = 0; i < ZeroesToAdd; i++)
	{
		Data.Push(0x00);
	}
}