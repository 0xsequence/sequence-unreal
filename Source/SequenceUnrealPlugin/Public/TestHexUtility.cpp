#include "HexUtility.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestHexUtility, "Public.TestHexUtility",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestHexUtility::RunTest(const FString& Parameters)
{
	auto HexString = IntToHexString(0x256);
	
	if(HexStringToInt(HexString) != 0x256)
	{
		return false;
	}

	if(HexStringToInt("2629ad1f").GetValue() != 0x2629ad1f)
	{
		return false;
	}

	{
		auto Hash = HexStringToHash256("0x5");

		if(Hash == nullptr)
		{
			return false;
		}

		auto String = Hash256ToHexString(Hash);
		Hash = HexStringToHash256(String);
			
		if(!Hash256ToHexString(Hash).Equals(String))
		{
			return false;
		}
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
