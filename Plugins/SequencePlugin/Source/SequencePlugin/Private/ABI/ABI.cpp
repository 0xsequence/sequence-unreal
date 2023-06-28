#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4104)
#include "ABI/ABI.h"

#include "Types/BinaryData.h"
#include "HexUtility.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"

FNonUniformData NewEmptyBlock()
{
	auto block = FNonUniformData
	{
		new uint8[GBlockByteLength],
		GBlockByteLength
	};

	for(auto i = 0; i < GBlockByteLength; i++)
	{
		block.Arr[i] = 0x00;
	}

	return block;
}

FString TypeToString(EABIArgType Type)
{
	if(Type == STATIC) return "Static";
	if(Type == STRING) return "String";
	if(Type == BYTES) return "Bytes";
	return "Array";
}

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
		FABIArg* Arr = static_cast<FABIArg*>(this->Data);
		FABIArg Item = Arr[i];

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
		CopyInUint32(HeadPtr, Offset * GBlockByteLength);
		*Head = &HeadPtr[1 * GBlockByteLength];
		CopyInUint32(TailPtr, this->Length);
		
		auto RawData = static_cast<uint8*>(this->Data);
		
		for(auto i = 0; i < this->Length; i++)
		{
			// UE_LOG(LogTemp, Display, TEXT("the raw data at %i is %i"), i, RawData[i]);
			TailPtr[GBlockByteLength + i] = RawData[i];
		}
		
		*Tail = &TailPtr[GBlockByteLength * (this->GetBlockNum() - 1)];
		return;
	}

	if(this->Type == ARRAY)
	{
		CopyInUint32(HeadPtr, Offset * GBlockByteLength);
		*Head = &HeadPtr[1 * GBlockByteLength];
		CopyInUint32(TailPtr, this->Length);
		
		auto SubHead = &TailPtr[GBlockByteLength];
		auto SubTail = &TailPtr[GBlockByteLength * (this->Length + 1)];

		FABIArg* Arr = static_cast<FABIArg*>(this->Data);
		
		for(auto i = 0; i < this->Length; i++)
		{
			FABIArg Item = Arr[i];
			Item.Encode(&TailPtr[GBlockByteLength], &SubHead, &SubTail);
		}

		*Tail = &TailPtr[GBlockByteLength * (this->GetBlockNum() - 1)];
		return;
	}
}

FABIArg FABIArg::Decode(uint8* TrueStart, uint8* Start, uint8* Head)
{
	
	if(this->Type == STATIC)
	{
		this->Length = 1;
		this->Data = new uint8[GBlockByteLength];

		for(auto i = 0; i < GBlockByteLength; i++)
		{
			static_cast<uint8*>(this->Data)[i] = Head[i];
		}

		return *this;
	}

	
	auto Offset = CopyOutUint32(Head);
	auto DataPtr = &Start[Offset];

	if(this->Type == STRING || this->Type == BYTES)
	{
		this->Length = CopyOutUint32(DataPtr);
		auto NewData = new uint8[this->Length];

		for(auto i = 0; i < this->Length; i++)
		{
			NewData[i] = DataPtr[GBlockByteLength + i];
		}

		this->Data = NewData;
		return *this;
	}

	if(this->Type == ARRAY)
	{
		// Since Arrays/Lists are variable length, we use the type of the given arg to determine to expected type of the data
		// E.g. if we want a list of ints, we give a list(int) nested FABIArg structure
		// Then it finds the length, say its 3, and constructs a new arrays so we have a list(int, int, int) structure
		// It then decodes each of the children
		
		this->Length = CopyOutUint32(DataPtr);
		
		
		FABIArg* Arr = static_cast<FABIArg*>(this->Data);
		auto ModelArg = Arr[0];
		delete [] Arr;

		Arr = new FABIArg[this->Length];

		for(auto i = 0; i < this->Length; i++)
		{
			auto SubStart = &DataPtr[GBlockByteLength];
			auto SubHead = &DataPtr[GBlockByteLength * (i + 1)];
			Arr[i].Type = ModelArg.Type;

			// Must preserve our model
			if(ModelArg.Type == ARRAY)
			{
				Arr[i].Data = new FABIArg{*static_cast<FABIArg*>(ModelArg.Data)};
			}
			
			Arr[i] = Arr[i].Decode(TrueStart, SubStart, SubHead);
		}
		
		this->Data = Arr;
		
		return *this;
	}

	return FABIArg::Empty(STATIC); // Should never reach here
}

FNonUniformData FABIArg::ToBinary() const
{
	return FNonUniformData{static_cast<uint8*>(Data), Length};
}

FString FABIArg::ToHex() const
{
	return ToBinary().ToHex();
}

FString FABIArg::ToString() const
{
	return UTF8ToString(ToBinary());
}

uint8 FABIArg::ToUint8() const
{
	return static_cast<uint8*>(Data)[0];
}

FABIArg* FABIArg::ToArr() const
{
	return static_cast<FABIArg*>(Data);
}

void FABIArg::Log()
{
	if(this == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("This is Null!"));
	} else
	if(this->Data == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("%s This One is Null!"), *TypeToString(this->Type));
	}
	else if(this->Type == STATIC)
	{
		UE_LOG(LogTemp, Display, TEXT("Static: %s"), *HashToHexString(GBlockByteLength, static_cast<Hash>(this->Data)));
	}
	else if(this->Type == STRING)
	{
		UE_LOG(LogTemp, Display, TEXT("String: %s"), *UTF8ToString(this->ToBinary()));
	}
	else if(this->Type == BYTES)
	{
		UE_LOG(LogTemp, Display, TEXT("Static: %s"), *HashToHexString(this->Length, static_cast<Hash>(this->Data)));
	} else
	{
		UE_LOG(LogTemp, Display, TEXT("Array of size %i:"), this->Length);

		for(auto i = 0; i < this->Length; i++)
		{
			this->ToArr()[i].Log();
		}
	}
}

void FABIArg::Destroy()
{
	if(Type == ARRAY)
	{
		delete [] static_cast<FABIArg*>(Data);
	}
	else
	{
		delete [] static_cast<uint8*>(Data);
	}
}

FABIArg FABIArg::Empty(EABIArgType Type)
{
	return FABIArg{Type, 0, nullptr};
}

FABIArg FABIArg::New(FString Value)
{
	auto Data = StringToUTF8(Value);
	return FABIArg{STRING, Data.Length, Data.Arr};
}

FABIArg FABIArg::New(uint8 Value)
{
	auto Ptr = new uint8[1];
	Ptr[0] = Value;
	return FABIArg{STATIC, 1, Ptr};
}

FABIArg FABIArg::New(uint32 Value)
{
	auto Ptr = new uint8[4];
	CopyInUint32(Ptr, Value);
	return FABIArg{STATIC, 4, Ptr};
}

FABIArg FABIArg::New(FABIArg* Array, uint32 Length)
{
	return FABIArg{ARRAY, Length, Array};
}

FNonUniformData ABI::EncodeArgs(FString Method, FABIArg* Args, uint8 ArgNum)
{
	auto BlockNum = 0;
	
	for(auto i = 0; i < ArgNum; i++)
	{
		auto Arg = Args[i];
		BlockNum += Arg.GetBlockNum();
	}

	uint32 TotalByteLength = (GMethodIdByteLength + GBlockByteLength * BlockNum);
	uint8* Blocks = new uint8[TotalByteLength];

	for(auto i = 0; i < TotalByteLength; i++)
	{
		Blocks[i] = 0;
	}

	auto Signature = FHash256::New();
	auto Msg = StringToUTF8(Method);
	Keccak256::getHash(Msg.Arr, Msg.GetLength(), Signature.Arr);

	for(auto i = 0; i < GMethodIdByteLength; i++)
	{
		Blocks[i] = Signature.Arr[i];
	}

	// Free calculation data
	Signature.Destroy();
	Msg.Destroy();
	
	uint8* HeadPtr = &Blocks[GMethodIdByteLength];
	uint8* Start = HeadPtr;
	uint8* TailPtr = &Blocks[GMethodIdByteLength + GBlockByteLength * ArgNum];

	for(auto i = 0; i < ArgNum; i++)
	{
		auto Arg = Args[i];
		Arg.Encode(Start, &HeadPtr, &TailPtr);
	}

	return FNonUniformData{
		Blocks, TotalByteLength
	};
}

FNonUniformData ABI::Encode(const FString Method, TArray<FABIProperty*>& Args)
{
	const auto Size = Args.Num();
	FABIArg Arr[Size];

	for(auto i = 0; i < Size; i++)
	{
		Arr[i] = Args[i]->Serialize();
	}

	return EncodeArgs(Method, Arr, Size);
}

void ABI::DecodeArgs(FNonUniformData Data, FABIArg* Args, uint8 ArgNum)
{
	for(auto i = 0; i < ArgNum; i++)
	{
		uint8* Start = &Data.Arr[GMethodIdByteLength];
		uint8* Head = &Data.Arr[GMethodIdByteLength + (i * GBlockByteLength)];
		Args[i] = Args[i].Decode(Data.Arr, Start, Head);
	}
}



void CopyInUint32(uint8* BlockPtr, uint32 Value)
{
	for(auto i = 0; i < 4; i++)
	{
		BlockPtr[GBlockByteLength - i - 1] = Value >> (8 * i);
	}
}

uint32 CopyOutUint32(uint8* BlockPtr)
{
	uint32 Value = 0;

	for(auto i = 0; i < 4; i++)
	{
		Value += (BlockPtr[GBlockByteLength - 1 - i]) << i * 8;
	}

	return Value;
}

