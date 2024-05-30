// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Types/BinaryData.h"
#include "Util/HexUtility.h"
#include "Types/Types.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestCryptoTypes, "Public.Tests.TestCryptoTypes",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestCryptoTypes::RunTest(const FString& Parameters)
{
	TStaticArray<uint8, 32> Arr;
	for(auto i = 0; i < 32; i++) Arr[i] = 0x00;
	Arr[31] = 0x01;
	Uint256 Test = Uint256(Arr.GetData());
	uint8* TestPtr = reinterpret_cast<uint8*>(Test.value);

	TStaticArray<uint8, 32> TestArr;
	for(int i = 0; i < 32; i++) TestArr[i] = TestPtr[i];
	
	UE_LOG(LogTemp, Display, TEXT("%s"), *FHash256::From(TestArr).ToHex());
	Test.getBigEndianBytes(Arr.GetData());
	UE_LOG(LogTemp, Display, TEXT("%s"), *FHash256::From(Arr).ToHex());
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
