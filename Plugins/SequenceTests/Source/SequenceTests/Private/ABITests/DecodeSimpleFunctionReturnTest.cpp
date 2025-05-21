#include "CoreMinimal.h"
#include "Types/ERC20.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDecodeSimpleFunctionReturnTest, "SequencePlugin.EndToEnd.ABITests.DecodeSimpleFunctionReturnTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDecodeSimpleFunctionReturnTest::RunTest(const FString& Parameters)
{
	const FString AbiJson = TEXT(R"({
	  "type": "function",
	  "name": "getSupply",
	  "inputs": [],
	  "outputs": [
	    { "name": "supply", "type": "uint256" }
	  ],
	  "stateMutability": "view"
	})");

	const FString EncodedData = "0x0000000000000000000000000000000000000000000000000000000000001388";
	const FString Decoded = USequenceSupport::DecodeFunctionResult(AbiJson, EncodedData);

	AddInfo(FString::Printf(TEXT("Decoded Data: %s"), *Decoded));
    return true;
} 