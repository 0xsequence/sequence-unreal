#include "CoreMinimal.h"
#include "Types/ERC721SaleContract.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTokenWrapper_ERC721Sale_Create, "SequencePlugin.UnitTests.TokenWrapper.ERC721Sale_Create", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTokenWrapper_ERC721Sale_Create::RunTest(const FString& Parameters)
{
    const FString TestAddress = "0x1234567890123456789012345678901234567890";
    const FString TestPaymentToken = "0x0987654321098765432109876543210987654321";
    const int32 TestMaxTotal = 1000;
    const FString TestData = "test_data";
    
    UERC721SaleContract* Contract = UERC721SaleContract::Create(TestAddress, TestPaymentToken, TestMaxTotal, TestData);
    
    TestNotNull("Contract should not be null", Contract);
    TestEqual("Contract address should match", Contract->ContractAddress, TestAddress);
    TestEqual("Payment token should match", Contract->PaymentToken, TestPaymentToken);
    TestEqual("Max total should match", Contract->MaxTotal, TestMaxTotal);
    TestEqual("Data should match", Contract->Data, TestData);
    
    return true;
} 