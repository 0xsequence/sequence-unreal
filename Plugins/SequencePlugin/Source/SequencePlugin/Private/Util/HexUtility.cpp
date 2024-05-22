// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma warning(disable: 4018)
#pragma warning(disable: 4146)
#pragma warning(disable: 4104)
#include "Util/HexUtility.h"
#include "Types/BinaryData.h"

FString RandomHexCharacter()
{
	return HexLUTNew[FMath::RandRange(0, HexLUTSize-1)];
}

FString RandomHexByte()
{
	return RandomHexCharacter() + RandomHexCharacter();
}

uint8 RandomByte()
{
	uint8* byte = new uint8[1];
	HexToBytes(RandomHexByte(), byte);
	uint8 res = byte[0];
	delete[] byte;
	return res;
}

TArray<uint8_t> PKCS7(FString in)
{
	FUnsizedData ByteBuff = StringToUTF8(in);

	int32 ByteLength = ByteBuff.GetLength();
	int32 ModLength = ByteLength % 16;

	uint8_t PaddingByte = GetPaddingByte(ByteLength);

	TArray<uint8_t> TBuff;
	TBuff.Reserve(ByteLength + (16-ModLength));
	TBuff.Append(ByteBuff.Arr.Get()->GetData(),ByteBuff.GetLength());

	for (int i = 0; i < (16 - ModLength); i++)
	{
		TBuff.Add(PaddingByte);
	}

	return TBuff;
}

uint8_t GetPaddingByte(int32 StringLength)
{
	uint8_t Byte = 0x00;
	int32 ModLength = StringLength % 16;
	switch (ModLength)
	{
	case 0:
		Byte = 0x10;
		break;
	case 1:
		Byte = 0x0F;
		break;
	case 2:
		Byte = 0x0E;
		break;
	case 3:
		Byte = 0x0D;
		break;
	case 4:
		Byte = 0x0C;
		break;
	case 5:
		Byte = 0x0B;
		break;
	case 6:
		Byte = 0x0A;
		break;
	case 7:
		Byte = 0x09;
		break;
	case 8:
		Byte = 0x08;
		break;
	case 9:
		Byte = 0x07;
		break;
	case 10:
		Byte = 0x06;
		break;
	case 11:
		Byte = 0x05;
		break;
	case 12:
		Byte = 0x04;
		break;
	case 13:
		Byte = 0x03;
		break;
	case 14:
		Byte = 0x02;
		break;
	case 15:
		Byte = 0x01;
		break;
	}

	return Byte;
}

FString IntToHexString(uint64 Num)
{
	if (Num == 0)
	{
		return "0x0";
	}

	FString String = "";

	while (Num > 0)
	{
		const uint64 Remainder = Num & 0xf;

		auto Numeral = IntToHexLetter(Remainder);

		String = Numeral + String;

		Num -= Remainder;
		Num = Num >> 4;
	}

	return "0x" + String;
}

FString IntToHexLetter(uint8 Num)
{
	Num &= 0xf;

	if (Num <= 9)
	{
		return FString::FromInt(Num);
	}

	if (Num == 0xa)
	{
		return "a";
	}

	if (Num == 0xb)
	{
		return "b";
	}

	if (Num == 0xc)
	{
		return "c";
	}

	if (Num == 0xd)
	{
		return "d";
	}

	if (Num == 0xe)
	{
		return "e";
	}

	if (Num == 0xf)
	{
		return "f";
	}

	// Should be impossible to get here
	return "";
}

TOptional<uint8> HexLetterToInt(const TCHAR Hex)
{
	switch (Hex)
	{
		case '0':
			return TOptional<uint8>(0);
		case '1':
			return TOptional<uint8>(1);
		case '2':
			return TOptional<uint8>(2);
		case '3':
			return TOptional<uint8>(3);
		case '4':
			return TOptional<uint8>(4);
		case '5':
			return TOptional<uint8>(5);
		case '6':
			return TOptional<uint8>(6);
		case '7':
			return TOptional<uint8>(7);
		case '8':
			return TOptional<uint8>(8);
		case '9':
			return TOptional<uint8>(9);
		case 'A':
		case 'a':
			return TOptional<uint8>(10);
		case 'B':
		case 'b':
			return TOptional<uint8>(11);
		case 'C':
		case 'c':
			return TOptional<uint8>(12);
		case 'D':
		case 'd':
			return TOptional<uint8>(13);
		case 'E':
		case 'e':
			return TOptional<uint8>(14);
		case 'F':
		case 'f':
			return TOptional<uint8>(15);
		default:
			return TOptional<uint8>();
	}
}

TOptional<uint64> HexStringToUint64(FString Hex)
{
	uint64 Sum = 0;
	uint64 Offset = 0; // Compensates for 0x prefix

	if(Hex.StartsWith("0x"))
	{
		Offset = 2;
	}

	for(int Counter = 0; Counter < Hex.Len() - Offset; Counter++)
	{
		const TCHAR Letter = Hex[Hex.Len() - 1 - Counter];
		TOptional<uint8> Convert = HexLetterToInt(Letter);

		if(!Convert.IsSet())
		{
			return TOptional<uint64>();
		}

		const uint64 Converted = Convert.GetValue();
		Sum += Converted << 4 * Counter;
	}

	return Sum;
}

TArray<uint8> HexToBytesInline(FString in)
{
	in.RemoveFromStart("0x");

	const uint32 length = (in.Len() / 2) + (in.Len() % 2);
	uint8* Arr = new uint8[length];
	HexToBytes(in, Arr);

	TArray<uint8> FinalArr;
	FinalArr.Append(Arr, length);
	delete [] Arr;
	
	return FinalArr;
}

FString TrimHex(FString Hex)
{
	if(Hex.Len() == 0)
	{
		return "0";
	}
	
	FString HexCopy = FString(Hex);
	
	HexCopy.RemoveFromStart("0x");

	while(HexCopy[0] == '0')
	{
		HexCopy.RemoveFromStart("0");
		
		if(HexCopy.Len() == 0)
		{
			return "0";
		}
	}

	return HexCopy;
}

TArray<uint8> MakeArray(uint8* Ptr, int Len)
{
	TArray<uint8> arr;
	for(int i = 0; i < Len; i++) arr.Push(Ptr[i]);
	return arr;
}
