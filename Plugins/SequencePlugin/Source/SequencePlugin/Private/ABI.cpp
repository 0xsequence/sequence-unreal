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
			auto Data = static_cast<uint8*>(this->Data);
			HeadPtr[GBlockByteLength - this->Length + i] = Data[i];
		}
		
		*Head = &HeadPtr[1 * GBlockByteLength];
		return;
	}

	if(this->Type == STRING || this->Type == BYTES)
	{
		
		
		for(auto i = 0; i < this->Length; i++)
		{
			
		}
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

	uint32 BlockByteLength = (GMethodIdByteLength + GBlockByteLength * BlockNum);
	uint8 Blocks[BlockByteLength];

	for(auto i = 0; i < BlockByteLength; i++)
	{
		Blocks[i] = 0;
	}

	uint8* HeadPtr = &Blocks[GMethodIdByteLength];
	uint8* TailPtr = &Blocks[GMethodIdByteLength + GBlockByteLength * ArgNum];

	for(auto i = 0; i < ArgNum; i++)
	{
		
	}
}

FBinaryData String_to_UTF8(FString String)
{
	const TCHAR* Chars = String.GetCharArray().GetData();
	const uint8 ByteLength = FCString::Strlen(Chars);
	uint8* Data = reinterpret_cast<uint8*>(TCHAR_TO_UTF8(Chars));

	return FBinaryData{
		Data, ByteLength
	};
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
