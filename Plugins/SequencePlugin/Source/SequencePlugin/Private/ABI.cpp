#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4104)
#include "ABI.h"

#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"

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
		auto RawData = static_cast<uint8*>(this->Data);
		
		for(auto i = 0; i < this->Length; i++)
		{
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
		
		auto RawData = static_cast<uint8*>(this->Data);
		
		for(auto i = 0; i < this->Length; i++)
		{
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
		auto SubTail = &TailPtr[GBlockByteLength * (this->Length + 1)];

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

void FABIArg::Decode(uint8* Start, uint8** Head, uint8** Tail)
{
	auto HeadPtr = *Head;
	auto TailPtr = *Tail;
	
	if(this->Type == STATIC)
	{
		auto RawData = static_cast<uint8*>(this->Data);
		
		for(auto i = 0; i < this->Length; i++)
		{
			RawData[this->Length - 1 - i] = HeadPtr[GBlockByteLength - 1 - i];
		}

		*Head = &HeadPtr[1 * GBlockByteLength];
		return;
	}

	auto Offset = (TailPtr - Start) / GBlockByteLength;

	if(this->Type == STRING || this->Type == ARRAY)
	{
		CopyUInt32(HeadPtr, Offset * GBlockByteLength);
		*Head = &HeadPtr[1 * GBlockByteLength];
		CopyUInt32(TailPtr, this->Length);
		
		auto RawData = static_cast<uint8*>(this->Data);
		
		for(auto i = 0; i < GBlockByteLength; i++)
		{
			auto Entry = TailPtr[GBlockByteLength + i];

			if(Entry == 0) return;
			
			RawData[i] = Entry;
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
		auto SubTail = &TailPtr[GBlockByteLength * (this->Length + 1)];

		FABIArg** Arr = static_cast<FABIArg**>(this->Data);
		
		for(auto i = 0; i < this->Length; i++)
		{
			FABIArg Item = *Arr[i];
			Item.Decode(&TailPtr[GBlockByteLength], &SubHead, &SubTail);
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

	Hash256 Signature = new uint8[GHash256ByteLength];
	auto Msg = String_to_UTF8(Method);
	Keccak256::getHash(Msg.Data, Msg.ByteLength, Signature);

	for(auto i = 0; i < GMethodIdByteLength; i++)
	{
		Blocks[i] = Signature[i];
	}

	// Free calculation data
	delete [] Signature;
	delete [] Msg.Data;
	
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

void ABI::Decode(FBinaryData Data, FABIArg** Args, uint8 ArgNum)
{
	uint8* HeadPtr = &Data.Data[GMethodIdByteLength];
	uint8* Start = HeadPtr;
	uint8* TailPtr = &Data.Data[GMethodIdByteLength + GBlockByteLength * ArgNum];

	for(auto i = 0; i < ArgNum; i++)
	{
		auto Arg = Args[i];
		Arg->Decode(Start, &HeadPtr, &TailPtr);
	}
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
