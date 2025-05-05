#include "CoreMinimal.h"
#include "Types/ERC1155.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTokenWrapper_ERC1155_Create, "SequencePlugin.UnitTests.TokenWrapper.ERC1155_Create", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTokenWrapper_ERC1155_Create::RunTest(const FString& Parameters)
{
    const FString TestAddress = "0x1234567890123456789012345678901234567890";
    const FString TestData = "test_data";
    UERC1155* Token = UERC1155::Create(TestAddress, TestData);
    
    TestNotNull("Token should not be null", Token);
    TestEqual("Contract address should match", Token->ContractAddress, TestAddress);
    TestEqual("Data should match", Token->Data, TestData);
    
    return true;
} 