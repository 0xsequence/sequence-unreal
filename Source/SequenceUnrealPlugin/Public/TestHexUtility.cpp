#include "HexUtility.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestHexUtility, "Public.TestHexUtility",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestHexUtility::RunTest(const FString& Parameters)
{
	if(!IntToHexString(1).GetValue().Equals("0x1"))
	{
		return false;
	}

	if(HexStringToInt("2629ad1f").GetValue() != 640265503)
	{
		return false;
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
