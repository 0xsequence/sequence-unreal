#include "ABI.h"
#include "HexUtility.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestABI, "Public.TestABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestABI::RunTest(const FString& Parameters)
{
	uint8 Number1 = 1;
	auto NumberArg1 = FABIArg{STATIC, 1, &Number1};

	uint8 Number2 = 2;
	auto NumberArg2 = FABIArg{ STATIC, 1, &Number2};

	uint8 Number3 = 3;
	auto NumberArg3 = FABIArg{ STATIC, 1, &Number3};

	FString String1 = "one";
	auto UTFString1 = String_to_UTF8(String1);
	auto StringArg1 = FABIArg{STRING, UTFString1.GetLength(), UTFString1.Arr};
	
	FString String2 = "two";
	auto UTFString2 = String_to_UTF8(String2);
	auto StringArg2 = FABIArg{STRING, UTFString2.GetLength(), UTFString2.Arr};
	
	FString String3 = "one";
	auto UTFString3 = String_to_UTF8(String3);
	auto StringArg3 = FABIArg{STRING, UTFString3.GetLength(), UTFString3.Arr};

	auto ArrayArg1 = FABIArg{ARRAY, 2, new void*[2]{&NumberArg1, &NumberArg2}};
	auto ArrayArg2 = FABIArg{ARRAY, 1, new void*[1]{&NumberArg3}};
	auto ArrayArg3 = FABIArg{ARRAY, 2, new void*[2]{&ArrayArg1, &ArrayArg2}};
	auto ArrayArg4 = FABIArg{ARRAY, 3, new void*[3]{&StringArg1, &StringArg2, &StringArg3}};
	
	FABIArg** Args = new FABIArg*[2];
	Args[0] = &ArrayArg3;
	Args[1] = &ArrayArg4;

	// ENCODING
	
	auto BlockNumInt = (*Args[0]).GetBlockNum() + (*Args[1]).GetBlockNum();
	FString BlockNum = FString::FromInt(BlockNumInt);

	auto Obj = ABI::Encode("test", Args, 2);
	
	UE_LOG(LogTemp, Display, TEXT("HEADER: %s"), *HashToHexString(GMethodIdByteLength, &Obj.Arr[0]));
	
	for(auto i = 0; i < BlockNumInt; i++)
	{
		auto Addr = GMethodIdByteLength + GBlockByteLength * i;
		UE_LOG(LogTemp, Display, TEXT("%i %s"), Addr, *HashToHexString(GBlockByteLength, &Obj.Arr[Addr]));
	}

	// DECODING STUBS
	uint8 DecodeNumber1 = 0;
	auto DecodeNumberArg1 = FABIArg{STATIC, 1, &Number1};

	uint8 DecodeNumber2 = 0;
	auto DecodeNumberArg2 = FABIArg{ STATIC, 1, &Number2};

	uint8 DecodeNumber3 = 0;
	auto DecodeNumberArg3 = FABIArg{ STATIC, 1, &Number3};

	FString DecodeString1 = "";
	auto DecodeUTFString1 = String_to_UTF8(String1);
	auto DecodeStringArg1 = FABIArg{STRING, UTFString1.GetLength(), UTFString1.Arr};
	
	FString DecodeString2 = "";
	auto DecodeUTFString2 = String_to_UTF8(String2);
	auto DecodeStringArg2 = FABIArg{STRING, UTFString2.GetLength(), UTFString2.Arr};
	
	FString DecodeString3 = "";
	auto DecodeUTFString3 = String_to_UTF8(String3);
	auto DecodeStringArg3 = FABIArg{STRING, UTFString3.GetLength(), UTFString3.Arr};

	auto DecodeArrayArg1 = FABIArg{ARRAY, 2, new void*[2]{&DecodeNumberArg1, &DecodeNumberArg2}};
	auto DecodeArrayArg2 = FABIArg{ARRAY, 1, new void*[1]{&DecodeNumberArg3}};
	auto DecodeArrayArg3 = FABIArg{ARRAY, 2, new void*[2]{&DecodeArrayArg1, &DecodeArrayArg2}};
	auto DecodeArrayArg4 = FABIArg{ARRAY, 3, new void*[3]{&DecodeStringArg1, &DecodeStringArg2, &DecodeStringArg3}};
	
	FABIArg** DecodeArgs = new FABIArg*[2];
	DecodeArgs[0] = &DecodeArrayArg3;
	DecodeArgs[1] = &DecodeArrayArg4;
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
