#include "ABI.h"
#include "HexUtility.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestABI, "Public.TestABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestABI::RunTest(const FString& Parameters)
{
	auto LOG_ENCODED = true;
	auto LOG_DECODED = true;

	// DATA
	
	uint8 Number1 = 1;
	auto NumberArg1 = FABIArg::New(Number1);

	uint8 Number2 = 2;
	auto NumberArg2 = FABIArg::New(Number2);

	uint8 Number3 = 3;
	auto NumberArg3 = FABIArg::New(Number3);

	FString String1 = "one";
	auto StringArg1 = FABIArg::New(String1);
	
	FString String2 = "two";
	auto StringArg2 = FABIArg::New(String2);
	
	FString String3 = "one";
	auto StringArg3 = FABIArg::New(String3);

	auto ArrayArg1 = FABIArg::New(new FABIArg[2]{NumberArg1, NumberArg2}, 2);
	auto ArrayArg2 = FABIArg::New(new FABIArg[1]{NumberArg3}, 1);
	auto ArrayArg3 = FABIArg::New(new FABIArg[2]{ArrayArg1, ArrayArg2}, 2);
	auto ArrayArg4 = FABIArg::New(new FABIArg[3]{StringArg1, StringArg2, StringArg3}, 3);
	
	FABIArg* Args = new FABIArg[2]{ArrayArg3, ArrayArg4};
	
	// ENCODING
	auto BlockNumInt = (Args[0]).GetBlockNum() + (Args[1]).GetBlockNum();
	FString BlockNum = FString::FromInt(BlockNumInt);
	auto Obj = ABI::Encode("test", Args, 2);
	
	if(LOG_ENCODED)
	{
		UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *HashToHexString(GMethodIdByteLength, &Obj.Arr[0]));
		for(auto i = 0; i < BlockNumInt; i++)
		{
			auto Addr = GMethodIdByteLength + GBlockByteLength * i;
			UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *HashToHexString(GBlockByteLength, &Obj.Arr[Addr]));
		}
	}

	// DECODING STUBS
	auto DecodeNumberArg1 = FABIArg::Empty(STATIC);

	auto DecodeStringArg1 = FABIArg::Empty(STRING);

	auto DecodeArrayArg1 = FABIArg{ARRAY, 1, new FABIArg[2]{DecodeNumberArg1}};
	auto DecodeArrayArg3 = FABIArg{ARRAY, 1, new FABIArg[2]{DecodeArrayArg1}};
	auto DecodeArrayArg4 = FABIArg{ARRAY, 1, new FABIArg[3]{DecodeStringArg1}};
	
	FABIArg* DecodeArgs = new FABIArg[]{DecodeArrayArg3, DecodeArrayArg4};
	
	ABI::Decode(Obj, DecodeArgs, 2);

	auto Arr1 = DecodeArgs[0];
	auto Arr2 = DecodeArgs[1];

	UE_LOG(LogTemp, Display, TEXT(" Arr 1 of size %i at %i"), Arr1.Length, (uint64) Arr1.Data);
	UE_LOG(LogTemp, Display, TEXT(" Arr 2 of size %i at %i"), Arr2.Length, (uint64) Arr2.Data);

	for(auto i = 0; i < Arr1.Length; i++)
	{
		auto NumArr = Arr1.ToArr()[0];

		UE_LOG(LogTemp, Display, TEXT("Num Arr of size %i at %i"), NumArr.Length, (uint64) NumArr.Data);
		
		for(auto j = 0; j < NumArr.Length; j++)
		{
			auto Num = NumArr.ToArr()[0];

			UE_LOG(LogTemp, Display, TEXT("Num Arg at offset %i"), (uint64) Num.Data);
		}
	}

	for(auto i = 0; i < Arr2.Length; i++)
	{
		auto StringArg = Arr2.ToArr()[i];
		
		UE_LOG(LogTemp, Display, TEXT("String Arg %i is %s"), i, *StringArg.ToString());
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
