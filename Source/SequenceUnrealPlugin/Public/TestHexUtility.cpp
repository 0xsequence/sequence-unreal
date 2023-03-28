#include "HexUtility.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestHexUtility, "Public.TestHexUtility",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestHexUtility::RunTest(const FString& Parameters)
{
	auto hexString = IntToHexString(0x256);
	UE_LOG(LogTemp, Display, TEXT("Hex String is %s"), *hexString);
	
	if(HexStringToInt(hexString) != 0x256)
	{
		return false;
	}

	if(HexStringToInt("2629ad1f").GetValue() != 0x2629ad1f)
	{
		return false;
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
