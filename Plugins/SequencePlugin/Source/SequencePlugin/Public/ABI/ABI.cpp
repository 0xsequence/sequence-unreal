#include "ABI.h"

#include "Eth/Crypto.h"
#include "Types/BinaryData.h"

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

TFixedABIData ABI::UInt32(uint32 Input)
{
	TArray<uint8> Arr;
	ABIEncodeable::PushEmptyBlock(Arr);
	ABIEncodeable::CopyInUInt32(Arr, Input, 0);
	return TFixedABIData(Arr);
}

TFixedABIData ABI::Int32(int32 Input)
{
	TArray<uint8> Arr;
	ABIEncodeable::PushEmptyBlock(Arr);
	ABIEncodeable::CopyInInt32(Arr, Input, 0);
	return TFixedABIData(Arr);
}

TFixedABIData ABI::Bool(bool Input)
{
	TArray<uint8> Arr;
	ABIEncodeable::PushEmptyBlock(Arr);
	if(Input)
	{
		Arr[GBlockByteLength - 1] = 0x00;
	}
	return TFixedABIData(Arr);
}

TDynamicABIData ABI::String(FString Input)
{
	TArray<uint8> Arr = StringToUTF8(Input).ToArray();
	return TDynamicABIData(Arr);
}

FUnsizedData ABI::Encode(FString Signature, TArray<ABIEncodeable*> Arr)
{
	TFixedABIArray FixedArr(Arr);
	FUnsizedData Data {
		FixedArr.Encode()
	};
	FUnsizedData SignatureData = StringToUTF8(Signature);
	FHash256 Hash = GetKeccakHash(SignatureData);
	Data.Arr->Insert(Hash.Ptr(), 4, 0);
	return Data;
}

FString ABI::Display(FString Signature, TArray<ABIEncodeable*> Arr)
{
	FString Str = "";
	FString Reference = Encode(Signature, Arr).ToHex();

	for(int i = 0; i < Reference.Len(); i++)
	{
		if(i % (GBlockByteLength * 2) == GSignatureLength * 2)
		{
			Str += '\n';
		}
		
		Str += Reference[i];
	}
	
	return Str;
}
