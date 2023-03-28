#include "HexUtility.h"

FString IntToHexString(uint64 Num)
{
	if(Num == 0)
	{
		return "0x0";
	}
	
	FString String = "";

	while (Num > 0)
	{
		auto Remainder = Num & 0xf;

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
	
	if(Num <= 9)
	{
		return FString::FromInt(Num);
	}

	if(Num == 0xa)
	{
		return "a";
	}

	if(Num == 0xb)
	{
		return "b";
	}

	if(Num == 0xc)
	{
		return "c";
	}

	if(Num == 0xd)
	{
		return "d";
	}

	if(Num == 0xe)
	{
		return "e";
	}

	if(Num == 0xf)
	{
		return "f";
	}

	// Should be impossible to get here
	return "";
}

FString Hash256ToHexString(Hash256 Hash)
{
	FString String = "0x";
	
	for(auto i = 0; i < 32; i++)
	{
		uint8 Byte = Hash[i];

		String = String + IntToHexLetter(Byte >> 4) + IntToHexLetter(Byte & 0xf);
	}

	return String;
}

TOptional<uint32> HexLetterToInt(TCHAR Hex)
{
	switch (Hex)
	{
		case '0':
			return TOptional<uint32>(0);
		case '1':
			return TOptional<uint32>(1);
		case '2':
			return TOptional<uint32>(2);
		case '3':
			return TOptional<uint32>(3);
		case '4':
			return TOptional<uint32>(4);
		case '5':
			return TOptional<uint32>(5);
		case '6':
			return TOptional<uint32>(6);
		case '7':
			return TOptional<uint32>(7);
		case '8':
			return TOptional<uint32>(8);
		case '9':
			return TOptional<uint32>(9);
		case 'a':
			return TOptional<uint32>(10);
		case 'b':
			return TOptional<uint32>(11);
		case 'c':
			return TOptional<uint32>(12);
		case 'd':
			return TOptional<uint32>(13);
		case 'e':
			return TOptional<uint32>(14);
		case 'f':
			return TOptional<uint32>(15);
		default:
			return TOptional<uint32>();
	}
}

TOptional<uint32> HexStringToInt(FString Hex)
{
	uint32 Sum = 0;
	uint32 Offset = 0; // Compensates for 0x prefix

	if(Hex.StartsWith("0x"))
	{
		Offset = 2;
	}

	for(int Counter = 0; Counter < Hex.Len() - Offset; Counter++)
	{
		auto Letter = Hex[Hex.Len() - 1 - Counter];
		auto Convert = HexLetterToInt(Letter);

		if(!Convert.IsSet())
		{
			return TOptional<uint32>();
		}

		Sum += Convert.GetValue() << 4 * Counter;
	}

	return Sum;
}
