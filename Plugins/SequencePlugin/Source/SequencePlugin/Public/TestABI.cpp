#include "ABI.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestABI, "Public.TestABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestABI::RunTest(const FString& Parameters)
{
	uint8 Number = 21;
	auto NumberArg = FABIArg{STATIC, 1, &Number};

	FString String = "test";
	auto UTFString = String_to_UTF8(String);
	auto StringArg = FABIArg{STRING, UTFString.ByteLength, &UTFString.Data};

	auto ArrayArg = FABIArg{ARRAY, 2, new void*[2]{&NumberArg, &StringArg}};
	
	FString BlockNum = FString::FromInt(ArrayArg.GetBlockNum());
	
	UE_LOG(LogTemp, Display, TEXT("RESULT: %s"), *BlockNum);
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
