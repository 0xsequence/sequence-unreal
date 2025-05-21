#include "CoreMinimal.h"
#include "ABITestData.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Sequence/SequenceWallet.h"
#include "Types/ERC1155SaleContract.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FABIDecodeERC1155SalePaymentDetailsTest, "SequencePlugin.EndToEnd.ABITests.DecodeERC1155SalePaymentDetailsTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneDecodeSaleDetails, UABITestData*, Data, FAutomationTestBase*, DecodeSaleDetailsTest);

bool FIsDoneDecodeSaleDetails::Update()
{
    return this->Data->IsDone;
}

void FABIDecodeERC1155SalePaymentDetailsTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Decode ERC1155 Sale Details Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FABIDecodeERC1155SalePaymentDetailsTest::RunTest(const FString& Parameters)
{
    const FString AbiJson = TEXT(R"({
      "type": "function",
      "name": "globalSaleDetails",
      "inputs": [],
      "outputs": [
        {
          "name": "",
          "type": "tuple",
          "internalType": "struct IERC1155SaleFunctions.SaleDetails",
          "components": [
            {
              "name": "cost",
              "type": "uint256",
              "internalType": "uint256"
            },
            {
              "name": "supplyCap",
              "type": "uint256",
              "internalType": "uint256"
            },
            {
              "name": "startTime",
              "type": "uint64",
              "internalType": "uint64"
            },
            {
              "name": "endTime",
              "type": "uint64",
              "internalType": "uint64"
            },
            {
              "name": "merkleRoot",
              "type": "bytes32",
              "internalType": "bytes32"
            }
          ]
        }
      ],
      "stateMutability": "view"
    })");
    
    const FString ContractAddress = "0x36617a93a0af8a5b7e6861ab2759d7c63666c8b9";
    const FString PaymentToken = "0xf3C3351D6Bd0098EEb33ca8f830FAf2a141Ea2E1";
    constexpr int32 MaxTotal = 1;
    const FString& Data = "0x00";

    UABITestData* TestData = NewObject<UABITestData>();
    UERC1155SaleContract* Sale = UERC1155SaleContract::Create(ContractAddress, PaymentToken, MaxTotal, Data);
    
    TestNotNull("Token should not be null", Sale);
    TestEqual("Contract address should match", Sale->ContractAddress, ContractAddress);

    const FContractCall CallGlobalSaleDetails = Sale->GetGlobalSaleDetails();

    const TFunction<void(FString)> OnSuccess = [this, TestData, AbiJson](const FString& EncodedReturn)
    {
      const FString Decoded = USequenceSupport::DecodeFunctionResult(AbiJson, EncodedReturn);
      
    	AddInfo(FString::Printf(TEXT("Received Encoded Data: %s"), *EncodedReturn));
    	AddInfo(FString::Printf(TEXT("Decoded Data: %s"), *Decoded));
      TestData->IsDone = true;
    };

    const TFunction<void(FSequenceError)> OnFailure = [this, TestData](const FSequenceError& Error)
    {
    	AddInfo(FString::Printf(TEXT("Error while getting sale details: %s"), *Error.Message));
      TestData->IsDone = true;
    };
    
    const USequenceWallet* Wallet = NewObject<USequenceWallet>();
    Wallet->Call(CallGlobalSaleDetails, OnSuccess, OnFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneDecodeSaleDetails(TestData, this));
    return true;
}