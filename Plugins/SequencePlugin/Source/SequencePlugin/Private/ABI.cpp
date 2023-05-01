#include "ABI.h"

uint8 FABIArg::GetBlockNum() const
{
	if(this->Type == STATIC)
	{
		return 1;
	} 

	auto BlockCount = 2;

	if(this->Type == STRING || this->Type == BYTES)
	{
		if(this->Length == 0) return 3; // Empty data still uses one block
		
		return BlockCount + (this->Length - 1 + GBlockByteLength) / GBlockByteLength;
	}

	// For Arrays we must count through elements
	for(auto i = 0; i < this->Length; i++)
	{
		FABIArg** Arr = static_cast<FABIArg**>(this->Data);
		FABIArg Item = *Arr[i];

		BlockCount += Item.GetBlockNum();
	}

	return BlockCount;
}

// Start is from where dynamic objects are offset
// Head is a pointer to the HeadPtr
// Tail is a pointer to the TailPtr
void FABIArg::Encode(uint8* Start, uint8** Head, uint8** Tail)
{
	auto HeadPtr = *Head;
	auto TailPtr = *Tail;

	if(this->Type == STATIC)
	{
		for(auto i = 0; i < this->Length; i++)
		{
			auto RawData = static_cast<uint8*>(this->Data);
			HeadPtr[GBlockByteLength - this->Length + i] = RawData[i];
		}
		
		*Head = &HeadPtr[1 * GBlockByteLength];
		return;
	}

	auto Offset = (TailPtr - Start) / GBlockByteLength;

	if(this->Type == STRING || this->Type == BYTES)
	{
		CopyUInt32(HeadPtr, Offset * GBlockByteLength);
		*Head = &HeadPtr[1 * GBlockByteLength];
		CopyUInt32(TailPtr, this->Length);
		
		
		for(auto i = 0; i < this->Length; i++)
		{
			auto RawData = static_cast<uint8*>(this->Data);
			TailPtr[GBlockByteLength + i] = RawData[i];
		}

		*Tail = &TailPtr[GBlockByteLength * (this->GetBlockNum() - 1)];
		return;
	}

	if(this->Type == ARRAY)
	{
		CopyUInt32(HeadPtr, Offset * GBlockByteLength);
		*Head = &HeadPtr[1 * GBlockByteLength];
		CopyUInt32(TailPtr, this->Length);
		
		auto SubHead = &TailPtr[GBlockByteLength];
		auto SubTail = &TailPtr[GMethodIdByteLength + GBlockByteLength * this->Length];

		FABIArg** Arr = static_cast<FABIArg**>(this->Data);
		
		for(auto i = 0; i < this->Length; i++)
		{
			FABIArg Item = *Arr[i];
			Item.Encode(&TailPtr[GBlockByteLength], &SubHead, &SubTail);
		}

		*Tail = &TailPtr[GBlockByteLength * (this->GetBlockNum() - 1)];
		return;
	}
}

FBinaryData ABI::Encode(FString Method, FABIArg** Args, uint8 ArgNum)
{
	auto BlockNum = 0;
	
	for(auto i = 0; i < ArgNum; i++)
	{
		auto Arg = Args[i];
		BlockNum += Arg->GetBlockNum();
	}

	uint32 TotalByteLength = (GMethodIdByteLength + GBlockByteLength * BlockNum);
	uint8* Blocks = new uint8[TotalByteLength];

	for(auto i = 0; i < TotalByteLength; i++)
	{
		Blocks[i] = 0;
	}
	
	uint8* HeadPtr = &Blocks[GMethodIdByteLength];
	uint8* Start = HeadPtr;
	uint8* TailPtr = &Blocks[GMethodIdByteLength + GBlockByteLength * ArgNum];

	for(auto i = 0; i < ArgNum; i++)
	{
		auto Arg = Args[i];
		Arg->Encode(Start, &HeadPtr, &TailPtr);
	}

	return FBinaryData{
		Blocks, TotalByteLength
	};
}

FBinaryData String_to_UTF8(FString String)
{
	uint32 Length = String.Len();

	auto binary = FBinaryData{
		new uint8[Length], Length
	};

	StringToBytes(String, binary.Data, Length);

	// I have no idea why I need to add 1 but it works
	for(auto i = 0; i < binary.ByteLength; i++)
	{
		binary.Data[i] = binary.Data[i] + 1;
	}

	return binary;
}

FString UTF8_to_String(FBinaryData BinaryData)
{
	TArray<uint8> Buffer;
	for(auto i = 0; i < BinaryData.ByteLength; i++)
	{
		Buffer.Add(BinaryData.Data[i]);
	}
	Buffer.Add('\0');
	return reinterpret_cast<const char*>(Buffer.GetData());
}

void CopyUInt32(uint8* Ptr, uint32 Data)
{
	for(auto i = 0; i < 4; i++)
	{
		Ptr[32 - i - 1] = Data >> (8 * i);
	}
}
