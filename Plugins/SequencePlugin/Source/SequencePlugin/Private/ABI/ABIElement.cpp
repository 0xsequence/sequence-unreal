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

TArray<TSharedPtr<ABIElement>> ABIElement::AsArray()
{
	checkf(false, TEXT("Not an array type!"));
	return TArray<TSharedPtr<ABIElement>>();
}

TArray<uint8> ABIElement::AsRawBinary()
{
	checkf(false, TEXT("Not a binary type!"));
	return TArray<uint8>();
}

FUnsizedData ABIElement::AsUnsizedBinary()
{
	return FUnsizedData { AsRawBinary() };
}

uint32 ABIElement::AsUInt32()
{
	checkf(false, TEXT("Not an integer type!"));
	return 0;
}

int32 ABIElement::AsInt32()
{
	checkf(false, TEXT("Not an integer type!"));
	return 0;
}

bool ABIElement::AsBool()
{
	checkf(false, TEXT("Not a boolean type!"));
	return false;
}

FString ABIElement::AsString()
{
	checkf(false, TEXT("Not a string type!"));
	return "";
}

FAddress ABIElement::AsAddress()
{
	checkf(false, TEXT("Not an address type!"));
	return FAddress::New();
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
	return (Data[BlockPosition + GBlockByteLength - 1] << 0)
	+ (Data[BlockPosition + GBlockByteLength - 2] << 8)
	+ (Data[BlockPosition + GBlockByteLength - 3] << 16)
	+ (Data[BlockPosition + GBlockByteLength - 4] << 24);
}

int32 ABIElement::CopyOutInt32(TArray<uint8>& Data, int BlockPosition)
{
	return (Data[BlockPosition + GBlockByteLength - 1] << 0)
	+ (Data[BlockPosition + GBlockByteLength - 2] << 8)
	+ (Data[BlockPosition + GBlockByteLength - 3] << 16)
	+ (Data[BlockPosition + GBlockByteLength - 4] << 24);
}

TFixedABIArray::TFixedABIArray() : MyData(TArray<ABIElement*>())
{
}

TFixedABIArray::TFixedABIArray(TArray<TSharedPtr<ABIElement>> MyData) : MyData(MyData)
{
}

TArray<TSharedPtr<ABIElement>> TFixedABIArray::AsArray()
{
	return MyData;
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

TSharedPtr<ABIElement> TFixedABIArray::Clone()
{
	TArray<TSharedPtr<ABIElement>> Data;

	for(auto i = 0; i < MyData.Num(); i++)
	{
		Data.Push(MyData[i]->Clone());
	}

	TFixedABIArray Fixed(Data);
	return MakeShared<TFixedABIArray>(Fixed);
}

void TFixedABIArray::Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition)
{
	uint32 Count = MyData.Num();
	
	for(uint32 i = 0; i < Count; i++)
	{
		MyData[i]->Decode(Data, HeadPosition + GBlockByteLength * i, HeadPosition);
	}
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

TArray<TSharedPtr<ABIElement>> TDynamicABIArray::AsArray()
{
	return MyData;
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

TSharedPtr<ABIElement> TDynamicABIArray::Clone()
{
	TArray<TSharedPtr<ABIElement>> Data;

	for(auto i = 0; i < MyData.Num(); i++)
	{
		Data.Push(MyData[i]->Clone());
	}

	TDynamicABIArray Dynamic(Data);
	return MakeShared<TDynamicABIArray>(Dynamic);
}

void TDynamicABIArray::Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition)
{
	
	uint32 Offset = CopyOutUInt32(Data, BlockPosition);
	uint32 Count = CopyOutUInt32(Data, HeadPosition + Offset);

	TSharedPtr<ABIElement> Prototype = MyData.Pop();
	
	for(uint32 i = 0; i < Count; i++)
	{
		TSharedPtr<ABIElement> Elem = Prototype->Clone();
		Elem->Decode(Data, HeadPosition + Offset + GBlockByteLength * (i + 1), HeadPosition + Offset + GBlockByteLength);
		MyData.Push(Elem);
	}
}

void TDynamicABIArray::Push(TSharedPtr<ABIElement> Arg)
{
	MyData.Push(Arg);
}

TFixedABIData::TFixedABIData(TArray<uint8> MyData) : MyData(MyData)
{
	
}

TFixedABIData::TFixedABIData(const int Size) : MyData(TArray<uint8>())
{
	for(int i = 0; i < Size; i++) MyData.Push(0x00);
}

TArray<uint8> TFixedABIData::AsRawBinary()
{
	return MyData;
}

uint32 TFixedABIData::AsUInt32()
{
	return CopyOutUInt32(MyData, 0);
}

int32 TFixedABIData::AsInt32()
{
	return CopyOutInt32(MyData, 0);
}

bool TFixedABIData::AsBool()
{
	return MyData[GBlockByteLength - 1] == 0x01;
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
	return MakeShared<TFixedABIData>(Fixed);
}

void TFixedABIData::Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition)
{
	for(auto i = 0; i < MyData.Num(); i++)
	{
		MyData[i] = Data[BlockPosition + i];
	}
}

TDynamicABIData::TDynamicABIData(TArray<uint8> MyData) : MyData(MyData)
{
}

TArray<uint8> TDynamicABIData::AsRawBinary()
{
	return MyData;
}

FString TDynamicABIData::AsString()
{
	return UTF8ToString(MyData);
}

FAddress TDynamicABIData::AsAddress()
{
	TStaticArray<uint8, FAddress::Size> Arr;

	for(auto i = 0; i < FAddress::Size; i++)
	{
		Arr[i] = MyData[i];
	}
	
	return FAddress::From(Arr);
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

	if (MyData.IsEmpty())
	{
		return;
	}
	
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
	return MakeShared<TDynamicABIData>(Dynamic);
}

void TDynamicABIData::Decode(TArray<uint8>& Data, int BlockPosition, int HeadPosition)
{
	uint32 Offset = CopyOutUInt32(Data, BlockPosition);
	uint32 Count = CopyOutUInt32(Data, HeadPosition + Offset);
	
	for(uint32 i = 0; i < Count; i++)
	{
		MyData.Push(Data[HeadPosition + Offset + GBlockByteLength + i]);
	}
}
