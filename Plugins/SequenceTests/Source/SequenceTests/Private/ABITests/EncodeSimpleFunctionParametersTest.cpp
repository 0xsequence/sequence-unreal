#include "CoreMinimal.h"
#include "Types/ERC20.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEncodeSimpleFunctionParametersTest, "SequencePlugin.EndToEnd.ABITests.EncodeSimpleFunctionParametersTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FEncodeSimpleFunctionParametersTest::RunTest(const FString& Parameters)
{
	const FString FunctionSignature = TEXT("balanceOf(address,uint256)");

	TArray<TSharedPtr<FJsonValue>> Values;
	Values.Add(MakeShareable(new FJsonValueString("0x6615e4e985bf0d137196897dfa182dbd7127f54f")));
	Values.Add(MakeShareable(new FJsonValueNumber(2)));

	const FString JsonValues = USequenceSupport::JsonValueListToParsableString(Values);
	const FString Encoded = USequenceSupport::EncodeFunctionCall(FunctionSignature, JsonValues);

	AddInfo(FString::Printf(TEXT("Simple Encoding Test: FunctionSignature: %s, Values: %s"), *FunctionSignature, *JsonValues));
	AddInfo(FString::Printf(TEXT("Encoded Data: %s"), *Encoded));
	
    return true;
} 