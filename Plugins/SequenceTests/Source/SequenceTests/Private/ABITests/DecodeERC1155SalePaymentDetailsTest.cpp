#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Sequence/SequenceWallet.h"
#include "Types/ERC1155SaleContract.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FABIDecodeERC1155SalePaymentDetailsTest, "SequencePlugin.EndToEnd.ABI.DecodeERC1155SalePaymentDetailsTest", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FABIDecodeERC1155SalePaymentDetailsTest::RunTest(const FString& Parameters)
{
    const FString ContractAddress = "0x1234567890123456789012345678901234567890";
    const FString PaymentToken = "0x1234567890123456789012345678901234567890";
    constexpr int32 MaxTotal = 1;
    const FString& Data = "0x00";
    
    UERC1155SaleContract* Sale = UERC1155SaleContract::Create(ContractAddress, PaymentToken, MaxTotal, Data);
    
    TestNotNull("Token should not be null", Sale);
    TestEqual("Contract address should match", Sale->ContractAddress, ContractAddress);

    const FContractCall CallGlobalSaleDetails = Sale->GetGlobalSaleDetails();

    const TFunction<void(FString)> OnSuccess = [](FString DecodedReturn)
    {
    };

    const TFunction<void(FSequenceError)> OnFailure = [](FSequenceError Error)
    {
    };
    
    const USequenceWallet* Wallet = NewObject<USequenceWallet>();
    Wallet->Call(CallGlobalSaleDetails, OnSuccess, OnFailure);
    
    return true;
} 