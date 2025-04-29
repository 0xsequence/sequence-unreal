#include "CoreMinimal.h"
#include "Types/ERC721.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTokenWrapper_ERC721_Create, "SequencePlugin.UnitTests.TokenWrapper.ERC721_Create", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FTokenWrapper_ERC721_Create::RunTest(const FString& Parameters)
{
    const FString TestAddress = "0x1234567890123456789012345678901234567890";
    UERC721* Token = UERC721::Create(TestAddress);
    
    TestNotNull("Token should not be null", Token);
    TestEqual("Contract address should match", Token->ContractAddress, TestAddress);
    
    return true;
} 