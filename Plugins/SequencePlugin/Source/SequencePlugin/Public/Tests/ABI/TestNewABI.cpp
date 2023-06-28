#include "Misc/AutomationTest.h"
#include "ABI/ABITypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestNewABI, "Public.Tests.TestNewABI",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestNewABI::RunTest(const FString& Parameters)
{
	uint8 Number1 = 1;
	auto NumberProp1 = FABIUIntProperty(Number1);

	uint8 Number2 = 2;
	auto NumberProp2 = FABIUIntProperty(Number2);

	uint8 Number3 = 3;
	auto NumberProp3 = FABIUIntProperty(Number3);

	FString String1 = "one";
	auto StringProp1 = FABIStringProperty(String1);
	
	FString String2 = "two";
	auto StringProp2 = FABIStringProperty(String2);
	
	FString String3 = "one";
	auto StringProp3 = FABIStringProperty(String3);

	auto ArrayProp1 = FABIArrayProperty(new TArray{NumberProp1, NumberProp2});
	auto ArrayProp2 = FABIArrayProperty(new TArray{NumberProp3});
	auto ArrayProp3 = FABIArrayProperty(new TArray{ArrayProp1, ArrayProp2});
	auto ArrayProp4 = FABIArrayProperty(new TArray{StringProp1, StringProp2, StringProp3});
	
	FABIArg* Args = new FABIArg[2]{ArrayArg3, ArrayArg4};
	
	// ENCODING
	auto BlockNumInt = (Args[0]).GetBlockNum() + (Args[1]).GetBlockNum();
	FString BlockNum = FString::FromInt(BlockNumInt);
	auto Obj = ABI::EncodeArgs("test", Args, 2);
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
