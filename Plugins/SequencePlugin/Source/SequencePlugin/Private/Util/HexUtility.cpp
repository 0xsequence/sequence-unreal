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
	TBuff.Append(ByteBuff.Arr,ByteBuff.GetLength());

	for (int i = 0; i < (16 - ModLength); i++)
	{
		TBuff.Add(PaddingByte);
	}
	delete[] ByteBuff.Arr;

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

int32 GetBytesInString(FString in)
{
	int32 Bytes = 0;

	for (auto c : in.GetCharArray())
	{
		Bytes += sizeof(c);
	}

	return Bytes;
}

//TArray<uint8_t> HexToHexCharList(FString in)
//{
//	TArray<uint8_t> result;
//	for (int i = 0; i < in.Len(); i++)
//		result.Add((unsigned char)in[i]);
//	return result;
//}

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

//deprecated and superceded by BytesToHex (Unreal provided Util)
//FString HashToHexString(ByteLength Size, uint8* Hash)
//{
//	FString String = "";
//	
//	for(uint32 i = 0; i < Size; i++)
//	{
//		const uint8 Byte = Hash[i];
//
//		FString Added = IntToHexLetter(Byte >> 4) + IntToHexLetter(Byte & 0xf);
//		String = String + Added;
//	}
//
//	//String.RemoveFromStart("0x");
//
//	return String;
//}

uint8* HexToBytesInline(FString in, uint32 size)
{
	uint8* bytes = new uint8[size];
	HexToBytes(in, bytes);
	return bytes;
}

//uint8* HexStringToBytes(ByteLength Size, FString Hex)
//{
//	const Hash Hash = new uint8[Size];
//	// Set it to 0s
//	for(uint32 i = 0; i < Size; i++)
//	{
//		Hash[i] = 0x00;
//	}
//
//	// Compensation for 0x
//	int Offset = 0;
//	if(Hex.StartsWith("0x"))
//	{
//		Offset = 2;
//	}
//
//	if(Hex.Len() % 2 == 1)
//	{
//		Hex.InsertAt(Offset, '0');
//	}
//	
//	for(int Counter = 0; Counter < Size; Counter++)
//	{
//		
//		if(Hex.Len() - 1 - 2 * Counter < Offset)
//		{
//			break;
//		}
//
//		TOptional<uint8> Lower = HexLetterToInt(Hex[Hex.Len() - 1 - 2 * Counter]);
//		TOptional<uint8> Upper = HexLetterToInt(Hex[Hex.Len() - 2 - 2 * Counter]);
//
//		if(!Lower.IsSet() || !Upper.IsSet())
//		{
//			delete[] Hash;
//			return nullptr;
//		}
//
//		const int LowerVal = (Upper.GetValue() << 4) & 0x000000F0;
//		const int UpperVal = Lower.GetValue() & 0x0000000F;
//		const uint32 Pos = Size - 1 - Counter;
//		Hash[Pos] = LowerVal + UpperVal;
//	}
//
//	return Hash;	
//}



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


