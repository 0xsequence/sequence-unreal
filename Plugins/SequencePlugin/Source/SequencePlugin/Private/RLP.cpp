#include "RLP.h"
#include "Types/BinaryData.h"
#include "HexUtility.h"

ByteLength GetByteLength(uint32 Length)
{
	auto bytes = 4;

	for(auto i = 0; i < 4; i++)
	{
		uint8 byte = (Length >> (8 * (3 - i))) & 0xFF;
		
		if(byte != 0x00)
		{
			return bytes;
		}

		bytes -= 1;
	}

	return bytes;
}

ByteLength RLPItem::CalculateLength()
{
	if(this->Type == LIST)
	{
		auto Args = static_cast<RLPItem*>(this->Data);

		LengthSum = 0;

		for(auto i = 0; i < this->Length; i++)
		{
			auto Arg = Args[i];
			LengthSum += Arg.CalculateLength();
		}
		
		if(this->LengthSum <= 55)
		{
			return 1 + LengthSum;
		}

		return 1 + GetByteLength(LengthSum) + LengthSum;
	}
	
	if(this->Type == BINARY)
	{
		auto RawData = static_cast<uint8*>(this->Data);

		if(this->Length == 0) // Shouldn't happen 
		{
			return 1;
		}
		
		if(this->Length == 1 && RawData[0] <= 127)
		{
			return 1;
		}

		if(this->Length <= 55)
		{
			return 1 + this->Length;
		}

		return 1 + GetByteLength(this->Length) + this->Length;
	}

	return 0;
}

void RLPItem::Encode(uint8* HeadPtr)
{

	if(this->Type == BINARY)
	{
		auto RawData = static_cast<uint8*>(this->Data);

		if(this->Length == 1 && RawData[0] == 0x00)
		{
			HeadPtr[0] = 0x80; // We serialize 0x0 as an empty string
			return;
		}
		
		if(this->Length == 1 && RawData[0] <= 127)
		{
			HeadPtr[0] = RawData[0];
			return;
		}

		if(this->Length <= 55)
		{
			HeadPtr[0] = 0x80 + this->Length;
			
			for(auto i = 0; i < this->Length; i++)
			{
				HeadPtr[1 + i] = RawData[i];
			}
			return;
		}

		auto LengthByteLength = GetByteLength(this->Length);

		HeadPtr[0] = 0xb7 + LengthByteLength;

		for(auto i = 0; i < LengthByteLength; i++)
		{
			HeadPtr[1 + i] = 0xFF & (this->Length >> 8 * (LengthByteLength - 1 - i));
		}

		for(auto i = 0; i < this->Length; i++)
		{
			HeadPtr[1 + LengthByteLength + i] = RawData[i];
		}

		return;
	}

	if(this->Type == LIST)
	{
		auto Args = static_cast<RLPItem*>(this->Data);

		auto DataOffset = 1;
		this->CalculateLength();
		
		if(LengthSum <= 55)
		{
			HeadPtr[0] = 0xc0 + LengthSum;
		}
		else
		{
			auto LengthByteLength = GetByteLength(LengthSum);
			HeadPtr[0] = 0xf7 + LengthByteLength;
			
			for(auto i = 0; i < LengthByteLength; i++)
			{
				HeadPtr[1 + i] = 0xFF & (LengthSum >> 8 * (LengthByteLength - 1 - i));
			}

			DataOffset += LengthByteLength;
		}

		for(auto i = 0; i < this->Length; i++)
		{
			auto Arg = Args[i];
			Arg.Encode(&HeadPtr[DataOffset]);
			DataOffset += Arg.CalculateLength();
		}
	}
}

RLPItem Itemize(FString String)
{
	return Itemize(StringToUTF8(String));
}

RLPItem Itemize(FBinaryData &Data)
{
	return Itemize(Data.Arr, Data.GetLength());
}

RLPItem Itemize(FNonUniformData Data)
{
	return Itemize(Data.Arr, Data.GetLength());
}

RLPItem Itemize(Hash Hash, ByteLength Length)
{
	return RLPItem{
		RLPItemType::BINARY, Length, 0, Hash,
	};
}

RLPItem Itemize(RLPItem* Items, uint32 Length)
{
	return RLPItem{
		LIST, Length, 0, Items,
	};
}
FNonUniformData RLP::Encode(RLPItem Item)
{
	auto Length = Item.CalculateLength();

	// Bugfix?
	//if(Length > 56) {
	//	Length += 1;
	//}
	
	uint8* Data = new uint8[Length];
	Item.Encode(Data);
	return FNonUniformData
	{
		Data, Length
	};
}
