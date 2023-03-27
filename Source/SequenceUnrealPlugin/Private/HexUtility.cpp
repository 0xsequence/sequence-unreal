#include "HexUtility.h"

TOptional<FString> IntToHexString(uint64 Num)
{
	if(Num == 0)
	{
		return TOptional<FString>("0x0");
	}
	
	FString String = "";

	while (Num > 0)
	{
		auto Remainder = Num % 16;

		auto Numeral = IntToHexLetter(Remainder);

		if(!Numeral.IsSet())
		{
			return TOptional<FString>();
		}

		String = Numeral.GetValue() + String;
		
		Num -= Remainder;
		Num /= 16;
	}
	
	return "0x" + String;
}

TOptional<FString> IntToHexLetter(uint Num)
{
	if(Num <= 9)
	{
		return FString::FromInt(Num);
	}

	if(Num == 10)
	{
		return TOptional<FString>("a");
	}

	if(Num == 11)
	{
		return TOptional<FString>("b");
	}

	if(Num == 12)
	{
		return TOptional<FString>("c");
	}

	if(Num == 13)
	{
		return TOptional<FString>("d");
	}

	if(Num == 14)
	{
		return TOptional<FString>("e");
	}

	if(Num == 15)
	{
		return TOptional<FString>("f");
	}

	return TOptional<FString>();
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
