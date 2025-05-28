#include "CoreMinimal.h"
#include "ABITestData.h"
#include "Types/ERC20.h"
#include "Misc/AutomationTest.h"
#include "SequencePlugin/Private/Provider.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FEncodeSimpleFunctionParametersTest, "SequencePlugin.EndToEnd.ABITests.EncodeSimpleFunctionParametersTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneEncodeSimpleFunctionParameters, UABITestData*, Data, FAutomationTestBase*, SimpleEncodingTest);

bool FIsDoneEncodeSimpleFunctionParameters::Update()
{
	return this->Data->IsDone;
}

void FEncodeSimpleFunctionParametersTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add(TEXT("Encode Simple Function Parameters Test"));
	OutTestCommands.Add(TEXT(""));
}

bool FEncodeSimpleFunctionParametersTest::RunTest(const FString& Parameters)
{
	UABITestData* TestData = NewObject<UABITestData>();
	const FString ContractAddress = TEXT("0xfd12c2e01f3261374697291880518888e0a739cc");
	const FString FunctionSignature = TEXT("balanceOf(address,uint256)");

	TArray<TSharedPtr<FJsonValue>> Values;
	Values.Add(MakeShareable(new FJsonValueString("0x6615e4e985bf0d137196897dfa182dbd7127f54f")));
	Values.Add(MakeShareable(new FJsonValueNumber(2)));

	const FString JsonValues = USequenceSupport::JsonValueListToParsableString(Values);
	const FString Encoded = USequenceSupport::EncodeFunctionCall(FunctionSignature, JsonValues);

	AddInfo(FString::Printf(TEXT("Simple Encoding Test: FunctionSignature: %s, Values: %s"), *FunctionSignature, *JsonValues));
	AddInfo(FString::Printf(TEXT("Encoded Data: %s"), *Encoded));

	FContractCall BalanceOfCall;
	BalanceOfCall.Data = Encoded;
	BalanceOfCall.To = ContractAddress;

	const TFunction<void(FString)> OnSuccess = [this, TestData](const FString& EncodedReturn)
	{
		const FString Abi = R"({"inputs": [{"internalType": "address","name": "account","type": "address"},{"internalType": "uint256","name": "id","type": "uint256"}],"name": "balanceOf","outputs": [{"internalType": "uint256","name": "","type": "uint256"}],"stateMutability": "view","type": "function"})";
		const FString DecodedData = USequenceSupport::DecodeFunctionResult(Abi, EncodedReturn);
		
		AddInfo(FString::Printf(TEXT("Decoded balanceOf() result: %s"), *DecodedData));
		TestData->IsDone = true;
	};

	const TFunction<void(FSequenceError)> OnFailure = [this, TestData](const FSequenceError& Error)
	{
		AddInfo(FString::Printf(TEXT("Error while calling balanceOf(): %s"), *Error.Message));
		TestData->IsDone = true;
	};

	UProvider* Provider = NewObject<UProvider>();
	Provider->Call(BalanceOfCall, EBlockTag::ELatest, OnSuccess, OnFailure);

	ADD_LATENT_AUTOMATION_COMMAND(FIsDoneEncodeSimpleFunctionParameters(TestData, this));
    return true;
} 