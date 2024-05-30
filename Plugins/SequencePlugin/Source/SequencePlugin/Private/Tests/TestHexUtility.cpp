// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Types/BinaryData.h"
#include "Util/HexUtility.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestHexUtility, "Public.TestHexUtility",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestHexUtility::RunTest(const FString& Parameters)
{
	auto HexString = IntToHexString(0x256);
	
	if(HexStringToUint64(HexString) != 0x256)
	{
		return false;
	}

	if(HexStringToUint64("2629ad1f").GetValue() != 0x2629ad1f)
	{
		return false;
	}

	{
		auto Hash = HexStringToBinary("0x5");

		if(Hash.Arr == nullptr)
		{
			return false;
		}

		auto String = Hash.ToHex();
		Hash = HexStringToBinary(String);
			
		if(!Hash.ToHex().Equals(String))
		{
			return false;
		}
	}
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
