#include "BinaryData.h"
#include "HexUtility.h"
#include "Types.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestCryptoTypes, "Public.Tests.TestCryptoTypes",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestCryptoTypes::RunTest(const FString& Parameters)
{
	uint8 arr[32];
	for(auto i = 0; i < 32; i++) arr[i] = 0x00;
	arr[31] = 0x01;
	auto test = Uint256(arr);
	UE_LOG(LogTemp, Display, TEXT("%s"), *FHash256::From(reinterpret_cast<uint8*>(test.value)).ToHex());
	test.getBigEndianBytes(arr);
	UE_LOG(LogTemp, Display, TEXT("%s"), *FHash256::From(arr).ToHex());
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
