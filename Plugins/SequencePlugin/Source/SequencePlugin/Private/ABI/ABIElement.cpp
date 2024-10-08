// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "ABI/ABIElement.h"
#include "ABI/ABI.h"

void ABIElement::PushEmptyBlock(TArray<uint8>& Data)
{
	for(int i = 0; i < GBlockByteLength; i++)
	{
		Data.Push(0x00);
	}
}

void ABIElement::CopyInUInt32(TArray<uint8>& Data, uint32 Value, int BlockPosition)
{
	for(int i = 0; i < 4; i++)
	{
		Data[BlockPosition + GBlockByteLength - 1 - i] = Value >> 8 * i;
	}
}

void ABIElement::CopyInInt32(TArray<uint8>& Data, int32 Value, int BlockPosition)
{
	for(int i = 0; i < 4; i++)
	{
		Data[BlockPosition + GBlockByteLength - 1 - i] = Value >> 8 * i;
	}
}

uint32 ABIElement::CopyOutUInt32(TArray<uint8>& Data, int BlockPosition)
{
	return Data[BlockPosition + GBlockByteLength - 1] << 0
	+ Data[BlockPosition + GBlockByteLength - 2] << 8
	+ Data[BlockPosition + GBlockByteLength - 3] << 16
	+ Data[BlockPosition + GBlockByteLength - 4] << 24;
}

int32 ABIElement::CopyOutInt32(TArray<uint8>& Data, int BlockPosition)
{
	return Data[BlockPosition + GBlockByteLength - 1] << 0
	+ Data[BlockPosition + GBlockByteLength - 2] << 8
	+ Data[BlockPosition + GBlockByteLength - 3] << 16
	+ Data[BlockPosition + GBlockByteLength - 4] << 24;
}

TFixedABIArray::TFixedABIArray() : MyData(TArray<ABIElement*>())
{
}

TFixedABIArray::TFixedABIArray(TArray<TSharedPtr<ABIElement>> MyData) : MyData(MyData)
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

void TFixedABIArray::Push(TSharedPtr<ABIElement> Arg)
{
	MyData.Push(Arg);
}

TDynamicABIArray::TDynamicABIArray() : MyData(TArray<TSharedPtr<ABIElement>>())
{
}

TDynamicABIArray::TDynamicABIArray(TArray<TSharedPtr<ABIElement>> MyData) : MyData(MyData)
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

void TDynamicABIArray::Push(TSharedPtr<ABIElement> Arg)
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

TSharedPtr<ABIElement> TFixedABIData::Clone()
{
	TArray<uint8> Data;
	Data.Append(MyData);
	TFixedABIData Fixed(Data);
	return MakeShared<ABIElement>(Fixed);
}

void TFixedABIData::Decode(TArray<uint8>& Data, int BlockPosition)
{
	for(auto i = 0; i < MyData.Num(); i++)
	{
		MyData[i] = Data[BlockPosition + i];
	}
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

TSharedPtr<ABIElement> TDynamicABIData::Clone()
{
	TArray<uint8> Data;
	TDynamicABIData Dynamic(Data);
	return MakeShared<ABIElement>(Dynamic);
}

void TDynamicABIData::Decode(TArray<uint8>& Data, int BlockPosition)
{
	uint32 Count = CopyOutUInt32(Data, BlockPosition);
	for(auto i = 0; i < Count; i++)
	{
		MyData.Push(Data[BlockPosition + GBlockByteLength + i]);
	}
}
