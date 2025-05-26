#include "CoreMinimal.h"
#include "ABITestData.h"
#include "Misc/AutomationTest.h"
#include "Util/Async.h"
#include "Sequence/SequenceWallet.h"
#include "Types/ERC721SaleDetails.h"
#include "Types/ERC721SaleContract.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FABIDecodeERC721SaleDetailsTest, "SequencePlugin.EndToEnd.ABITests.DecodeERC721SaleDetailsTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/* Latent command used to poll off main thread to see if our requests are done */
DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FIsDoneDecodeERC721SaleDetails, UABITestData*, Data, FAutomationTestBase*, DecodeSaleDetailsTest);

bool FIsDoneDecodeERC721SaleDetails::Update()
{
    return this->Data->IsDone;
}

void FABIDecodeERC721SaleDetailsTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Decode ERC721 Sale Details Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FABIDecodeERC721SaleDetailsTest::RunTest(const FString& Parameters)
{
    const FString ContractAddress = "0x0b794c761d2116d3822a97b6bc85112b07951ea5";
    const FString Token = "0x0373e68a46e8168a2d1147c27df0cfe7a6baca73";
    constexpr int32 MaxTotal = 1;
    const FString& Data = "0x00";

    UABITestData* TestData = NewObject<UABITestData>();
    UERC721SaleContract* Sale = UERC721SaleContract::Create(ContractAddress, Token, MaxTotal, Data);
    
    TestNotNull("Sale should not be null", Sale);
    TestEqual("Contract address should match", Sale->ContractAddress, ContractAddress);

    const FContractCall CallSaleDetails = Sale->GetSaleDetails();

    const TFunction<void(FString)> OnSuccess = [this, TestData](const FString& EncodedReturn)
    {
		FERC721SaleDetails SaleDetails;
		SaleDetails.DecodeFromFunctionReturn(EncodedReturn);
      
    	AddInfo(FString::Printf(TEXT("Received Encoded Data: %s"), *EncodedReturn));
    	AddInfo(FString::Printf(TEXT("SaleDetails.SupplyCap: %d"), SaleDetails.SupplyCap));
    	AddInfo(FString::Printf(TEXT("SaleDetails.Cost: %s"), *SaleDetails.Cost));
    	AddInfo(FString::Printf(TEXT("SaleDetails.PaymentToken: %s"), *SaleDetails.PaymentToken));
    	AddInfo(FString::Printf(TEXT("SaleDetails.StartTime: %d"), SaleDetails.StartTime));
    	AddInfo(FString::Printf(TEXT("SaleDetails.EndTime: %d"), SaleDetails.EndTime));
    	AddInfo(FString::Printf(TEXT("SaleDetails.MerkleRoot: %s"), *SaleDetails.MerkleRoot));

    	TestEqual("SupplyCap should equal to 0", SaleDetails.SupplyCap, 0);
    	TestEqual("Cost should equal to 10000000000", SaleDetails.Cost, "10000000000");
    	TestEqual("PaymentToken should equal 0xf3c3351d6bd0098eeb33ca8f830faf2a141ea2e1", SaleDetails.PaymentToken, "0xf3c3351d6bd0098eeb33ca8f830faf2a141ea2e1");
    	TestEqual("StartTime should equal to 1748259960", SaleDetails.StartTime, 1748259960);
    	TestEqual("EndTime should equal to 1906112760", SaleDetails.EndTime, 1906112760);
    	TestEqual("MerkleRoot should be a zero hash", SaleDetails.MerkleRoot, "0x0000000000000000000000000000000000000000000000000000000000000000");
    	
		TestData->IsDone = true;
    };

    const TFunction<void(FSequenceError)> OnFailure = [this, TestData](const FSequenceError& Error)
    {
    	AddInfo(FString::Printf(TEXT("Error while getting sale details: %s"), *Error.Message));
      TestData->IsDone = true;
    };
    
    const USequenceWallet* Wallet = NewObject<USequenceWallet>();
    Wallet->Call(CallSaleDetails, OnSuccess, OnFailure);
    
    ADD_LATENT_AUTOMATION_COMMAND(FIsDoneDecodeERC721SaleDetails(TestData, this));
    return true;
}