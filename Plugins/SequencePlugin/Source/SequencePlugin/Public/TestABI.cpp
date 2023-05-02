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
	auto StringArg1 = FABIArg{STRING, UTFString1.ByteLength, UTFString1.Data};
	
	FString String2 = "two";
	auto UTFString2 = String_to_UTF8(String2);
	auto StringArg2 = FABIArg{STRING, UTFString2.ByteLength, UTFString2.Data};
	
	FString String3 = "one";
	auto UTFString3 = String_to_UTF8(String3);
	auto StringArg3 = FABIArg{STRING, UTFString3.ByteLength, UTFString3.Data};

	auto ArrayArg1 = FABIArg{ARRAY, 2, new void*[2]{&NumberArg1, &NumberArg2}};
	auto ArrayArg2 = FABIArg{ARRAY, 1, new void*[1]{&NumberArg3}};
	auto ArrayArg3 = FABIArg{ARRAY, 2, new void*[2]{&ArrayArg1, &ArrayArg2}};
	auto ArrayArg4 = FABIArg{ARRAY, 3, new void*[3]{&StringArg1, &StringArg2, &StringArg3}};

	auto BinData = HexStringtoBinary("0x77");
	auto BinArg = FABIArg{BYTES, BinData.ByteLength, BinData.Data};

	UE_LOG(LogTemp, Display, TEXT("Text: %s"), *UTF8_to_String(FBinaryData{
		static_cast<uint8*>(StringArg1.Data), StringArg1.Length
	}));
	
	FABIArg** Args = new FABIArg*[2];
	Args[0] = &ArrayArg3;
	Args[1] = &ArrayArg4;
	
	auto BlockNumInt = (*Args[0]).GetBlockNum() + (*Args[1]).GetBlockNum();
	FString BlockNum = FString::FromInt(BlockNumInt);
	
	UE_LOG(LogTemp, Display, TEXT("RESULT: %s"), *BlockNum);

	auto Obj = ABI::Encode("test", Args, 2);

	UE_LOG(LogTemp, Display, TEXT("BINARY: %s"), *HashToHexString(Obj.ByteLength, Obj.Data));

	for(auto i = 0; i < BlockNumInt; i++)
	{
		auto Addr = GMethodIdByteLength + GBlockByteLength * i;
		UE_LOG(LogTemp, Display, TEXT("BINARY: %s"), *HashToHexString(GBlockByteLength, &Obj.Data[Addr]));
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
