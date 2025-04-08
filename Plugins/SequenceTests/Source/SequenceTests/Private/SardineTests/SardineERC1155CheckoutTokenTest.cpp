#include "SardineERC1155CheckoutTokenTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Marketplace/Sardine/SardineCheckout.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "Util/Async.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FSardineERC1155CheckoutTokenTest, "SequencePlugin.EndToEnd.SardineTests.ERC1155CheckoutTokenTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FSardineERC1155CheckoutTokenTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Sardine ERC1155 Checkout Token Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FSardineERC1155CheckoutTokenTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting Sardine ERC1155 Checkout Token Test"));
    
    // Start PIE
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    
    // Wait for PIE to initialize
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));

    // Run the actual test
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        USardineERC1155CheckoutTokenTestHelper* TestHelper = NewObject<USardineERC1155CheckoutTokenTestHelper>();
        TestHelper->ParentTest = this;
        TestHelper->RunTest();

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForERC1155CheckoutToken(TestHelper));
        return true;
    }));
    return true;
}

void USardineERC1155CheckoutTokenTestHelper::RunTest()
{
    Checkout = NewObject<USardineCheckout>();

    // mock data
    int64 ChainID = 137;
    UERC1155SaleContract* SaleContract = NewObject<UERC1155SaleContract>();
    SaleContract->Initialize("0xe65b75eb7c58ffc0bf0e671d64d0e1c6cd0d3e5b", "0x3c499c542cEF5E3811e1192ce70d8cC03d5c3359", 99999, "");

    FString CollectionAddress = "0xdeb398f41ccd290ee5114df7e498cf04fac916cb";
    long TokenID = 1;
    long Amount = 1;
    FString RecipientAddress = "0x9886910Faec24328FE8104a300e5D08FF0B5675b";
    TArray<FString> Proof;
    TArray<uint8> Data;
    Proof.Empty();
    Data.Empty();

    const TSuccessCallback<FSardineNFTCheckout> GetNFTCheckoutTokenSuccess = [this](const FSardineNFTCheckout& Token)
    {
        bTestComplete = true;
        const FString Message = "Sardine NFT Checkout Token request succeeded with token";
        ParentTest->AddInfo(FString::Printf(TEXT("%s: %s"), *Message, *Token.Token));
    };

    const FFailureCallback GetNFTCheckoutTokenFailure = [this](const FSequenceError& Error)
    {
        bTestComplete = true;
        const FString Message = "Request Failure";
        ParentTest->AddError(FString::Printf(TEXT("%s: %s."), *Message, *Error.Message));
    };

    Checkout->SardineGetNFTCheckoutToken(ChainID, SaleContract, CollectionAddress, TokenID, Amount,
        GetNFTCheckoutTokenSuccess, GetNFTCheckoutTokenFailure, RecipientAddress, Data, Proof);
}

bool FWaitForERC1155CheckoutToken::Update()
{
    return TestHelper->IsTestComplete();
}
