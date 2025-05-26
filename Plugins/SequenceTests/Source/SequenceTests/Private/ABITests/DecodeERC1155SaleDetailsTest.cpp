#include "CoreMinimal.h"
#include "ABITestData.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Sequence/SequenceWallet.h"
#include "Types/ERC1155GlobalSaleDetails.h"
#include "Types/ERC1155SaleContract.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FABIDecodeERC1155SaleDetailsTest, "SequencePlugin.EndToEnd.ABITests.DecodeERC1155SaleDetailsTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneDecodeERC1155SaleDetails, UABITestData*, Data, FAutomationTestBase*, DecodeSaleDetailsTest);

bool FIsDoneDecodeERC1155SaleDetails::Update()
{
    return this->Data->IsDone;
}

void FABIDecodeERC1155SaleDetailsTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Decode ERC1155 Sale Details Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FABIDecodeERC1155SaleDetailsTest::RunTest(const FString& Parameters)
{
    const FString ContractAddress = "0x36617a93a0af8a5b7e6861ab2759d7c63666c8b9";
    const FString PaymentToken = "0xf3C3351D6Bd0098EEb33ca8f830FAf2a141Ea2E1";
    constexpr int32 MaxTotal = 1;
    const FString& Data = "0x00";

    UABITestData* TestData = NewObject<UABITestData>();
    UERC1155SaleContract* Sale = UERC1155SaleContract::Create(ContractAddress, PaymentToken, MaxTotal, Data);
    
    TestNotNull("Sale should not be null", Sale);
    TestEqual("Contract address should match", Sale->ContractAddress, ContractAddress);

    const FContractCall CallGlobalSaleDetails = Sale->GetGlobalSaleDetails();

    const TFunction<void(FString)> OnSuccess = [this, TestData](const FString& EncodedReturn)
    {
		FERC1155GlobalSaleDetails SaleDetails;
		SaleDetails.DecodeFromFunctionReturn(EncodedReturn);
      
    	AddInfo(FString::Printf(TEXT("Received Encoded Data: %s"), *EncodedReturn));
    	AddInfo(FString::Printf(TEXT("SaleDetails.Cost: %s"), *SaleDetails.Cost));
    	AddInfo(FString::Printf(TEXT("SaleDetails.SupplyCap: %d"), SaleDetails.SupplyCap));
    	AddInfo(FString::Printf(TEXT("SaleDetails.StartTime: %d"), SaleDetails.StartTime));
    	AddInfo(FString::Printf(TEXT("SaleDetails.EndTime: %d"), SaleDetails.EndTime));
    	AddInfo(FString::Printf(TEXT("SaleDetails.MerkleRoot: %s"), *SaleDetails.MerkleRoot));

    	TestEqual("Cost should equal to 0", SaleDetails.Cost, "0");
    	TestEqual("SupplyCap should equal to 10000", SaleDetails.SupplyCap, 10000);
    	TestEqual("StartTime should equal to 1747809900", SaleDetails.StartTime, 1747809900);
    	TestEqual("EndTime should equal to 1905662700", SaleDetails.EndTime, 1905662700);
    	TestEqual("MerkleRoot should be a zero hash", SaleDetails.MerkleRoot, "0x0000000000000000000000000000000000000000000000000000000000000000");
    	
		TestData->IsDone = true;
    };

    const TFunction<void(FSequenceError)> OnFailure = [this, TestData](const FSequenceError& Error)
    {
    	AddInfo(FString::Printf(TEXT("Error while getting sale details: %s"), *Error.Message));
      TestData->IsDone = true;
    };
    
    const USequenceWallet* Wallet = NewObject<USequenceWallet>();
    Wallet->Call(CallGlobalSaleDetails, OnSuccess, OnFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneDecodeERC1155SaleDetails(TestData, this));
    return true;
}