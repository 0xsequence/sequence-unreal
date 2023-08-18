#include "Types/BinaryData.h"
#include "HexUtility.h"


uint8* BlankArray(ByteLength Size)
{
	uint8* Arr = new uint8[Size];

	for(uint32 i = 0; i < Size; i++)
	{
		Arr[i] = 0x00;
	}

	return Arr;
}

void FBinaryData::Destroy()
{
	if(this->Arr != nullptr)
	{
		delete [] this->Arr;
		this->Arr = nullptr;
	}
}

FString FBinaryData::ToHex() const
{
	if(Arr == nullptr)
	{
		return "";
	}
	
	return HashToHexString(GetLength(), Arr);
}

void FBinaryData::Renew()
{
	this->Arr = BlankArray(this->GetLength());
}

FUnsizedData FUnsizedData::Empty()
{
	return FUnsizedData{nullptr, 0};
}

FUnsizedData::FUnsizedData(uint8* Arr, ByteLength Length) : Length(Length)
{
	this->Arr = Arr;
}

FUnsizedData FUnsizedData::Copy() const
{
	uint8* NewArr = new uint8[Length];
	for(uint32 i = 0; i < Length; i++)
	{
		NewArr[i] = Arr[i];
	}
	return FUnsizedData{NewArr, Length};
}

ByteLength FUnsizedData::GetLength() const
{
	return this->Length;
}

FUnsizedData FUnsizedData::Trim()
{
	FUnsizedData trimmed = Trimmed(*this);
	delete [] this->Arr;
	this->Arr = trimmed.Arr;
	return trimmed;
}

FUnsizedData FUnsizedData::From(const FString Hex)
{
	return HexStringToBinary(Hex);
}


FUnsizedData Trimmed(const FBinaryData& Data)
{
	uint32 Empty_Count = 0;
	const uint32 Length = Data.GetLength();

	for(uint32 i = 0; i < Length; i++)
	{
		if(Data.Arr[i] == 0x00)
		{
			Empty_Count += 1;
		}
		else
		{
			break;
		}
	}

	const ByteLength NewLength = Data.GetLength() - Empty_Count;

	if(NewLength == 0)
	{
		return FUnsizedData{new uint8[1]{0x00}, 1};
	}
	
	uint8* NewData = new uint8[NewLength];

	for(uint32 i = 0u; i < NewLength; i++)
	{
		NewData[i] = Data.Arr[i + Empty_Count];
	}

	return FUnsizedData(NewData, NewLength);
}

FHash256 FHash256::New()
{
	FHash256 Data = FHash256{};
	Data.Renew();
	return Data;
}

FHash256 FHash256::From(uint8* Arr)
{
	FHash256 Data = FHash256{};
	Data.Arr = Arr;
	return Data;
}

FHash256 FHash256::From(const FString Str)
{
	return From(HexStringToHash(Size, Str));
}

FAddress FAddress::New()
{
	FAddress Data = FAddress{};
	Data.Renew();
	return Data;
}

FAddress FAddress::From(uint8* Arr)
{
	FAddress Data = FAddress{};
	Data.Arr = Arr;
	return Data;
}

FAddress FAddress::From(FString Str)
{
	return From(HexStringToHash(Size, Str));
}

FPublicKey FPublicKey::New()
{
	FPublicKey Data = FPublicKey{};
	Data.Renew();
	return Data;
}

FPublicKey FPublicKey::From(uint8* Arr)
{
	FPublicKey Data = FPublicKey{};
	Data.Arr = Arr;
	return Data;
}

FPublicKey FPublicKey::From(FString Str)
{
	return From(HexStringToHash(Size, Str));
}


FPrivateKey FPrivateKey::New()
{
	FPrivateKey Data = FPrivateKey{};
	Data.Renew();
	return Data;
}

FPrivateKey FPrivateKey::From(uint8* Arr)
{
	FPrivateKey Data = FPrivateKey{};
	Data.Arr = Arr;
	return Data;
}

FPrivateKey FPrivateKey::From(FString Str)
{
	return From(HexStringToHash(Size, Str));
}

FBloom FBloom::New()
{
	FBloom Data = FBloom{};
	Data.Renew();
	return Data;
}

FBloom FBloom::From(uint8* Arr)
{
	FBloom Data = FBloom{};
	Data.Arr = Arr;
	return Data;
}

FBloom FBloom::From(FString Str)
{
	return From(HexStringToHash(Size, Str));
}

FBlockNonce FBlockNonce::New()
{
	FBlockNonce Data = FBlockNonce{};
	Data.Renew();
	return Data;
}

FBlockNonce FBlockNonce::From(uint8* Arr)
{
	FBlockNonce Data = FBlockNonce{};
	Data.Arr = Arr;
	return Data;
}

FBlockNonce FBlockNonce::From(const FString Str)
{
	return From(HexStringToHash(Size, Str));
}

void FBlockNonce::Increment() const
{
	for(uint32 i = 0; i < GetLength(); i--)
	{
		const int Index = GetLength() - 1 - i;
		this->Arr[Index] += 1;

		if(this->Arr[Index] != 0x00)
		{
			return;
		}
	}
}

FUnsizedData StringToUTF8(FString String)
{
	const uint32 Length = String.Len();

	FUnsizedData Binary = FUnsizedData{
		new uint8[Length], Length
	};

	StringToBytes(String, Binary.Arr, Length);

	// I have no idea why I need to add 1 but it works
	for(uint32 i = 0; i < Binary.GetLength(); i++)
	{
		Binary.Arr[i] = Binary.Arr[i] + 1;
	}

	return Binary;
}

FString UTF8ToString(FUnsizedData BinaryData)
{
	TArray<uint8> Buffer;
	for(uint32 i = 0; i < BinaryData.GetLength(); i++)
	{
		Buffer.Add(BinaryData.Arr[i]);
	}
	Buffer.Add('\0');
	return reinterpret_cast<const char*>(Buffer.GetData());
}
FUnsizedData HexStringToBinary(const FString Hex)
{
	FString HexCopy = FString(Hex);
	HexCopy.RemoveFromStart("0x");
	
	const uint32 Size = (HexCopy.Len() / 2) + (HexCopy.Len() % 2);
	
	return FUnsizedData(HexStringToHash(Size, Hex), Size);
}
