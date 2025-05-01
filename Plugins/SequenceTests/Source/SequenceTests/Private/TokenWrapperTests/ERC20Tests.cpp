#include "CoreMinimal.h"
#include "Types/ERC20.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTokenWrapper_ERC20_Create, "SequencePlugin.UnitTests.TokenWrapper.ERC20_Create", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTokenWrapper_ERC20_Create::RunTest(const FString& Parameters)
{
    const FString TestAddress = "0x1234567890123456789012345678901234567890";
    UERC20* Token = UERC20::Create(TestAddress);
    
    TestNotNull("Token should not be null", Token);
    TestEqual("Contract address should match", Token->ContractAddress, TestAddress);
    
    return true;
} 