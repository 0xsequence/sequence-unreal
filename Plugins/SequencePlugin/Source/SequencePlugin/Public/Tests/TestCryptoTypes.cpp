#include "Types/BinaryData.h"
#include "Util/HexUtility.h"
#include "Types/Types.h"
#include "Misc/AutomationTest.h"
#include "Bitcoin-Cryptography-Library/cpp/Uint256.hpp"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestCryptoTypes, "Public.Tests.TestCryptoTypes",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool TestCryptoTypes::RunTest(const FString& Parameters)
{
	TStaticArray<uint8, 32> arr;
	for(auto i = 0; i < 32; i++) arr[i] = 0x00;
	arr[31] = 0x01;
	Uint256 test = Uint256(arr.GetData());
	uint8* testPtr = reinterpret_cast<uint8*>(test.value);

	TStaticArray<uint8, 32> testArr;
	for(int i = 0; i < 32; i++) testArr[i] = testPtr[i];
	
	UE_LOG(LogTemp, Display, TEXT("%s"), *FHash256::From(testArr).ToHex());
	test.getBigEndianBytes(arr.GetData());
	UE_LOG(LogTemp, Display, TEXT("%s"), *FHash256::From(arr).ToHex());
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
