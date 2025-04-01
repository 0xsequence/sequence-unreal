#include "SardineGetNFTCheckoutTokenTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Marketplace/Sardine/SardineCheckout.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "Util/Async.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FSardineGetNFTCheckoutTokenTest, "SequencePlugin.EndToEnd.SardineTests.GetNFTCheckoutTokenTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FSardineGetNFTCheckoutTokenTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Sardine Get NFT Checkout Token Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FSardineGetNFTCheckoutTokenTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting Sardine Get NFT CHeckout Token Test"));
    
    // Start PIE
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    
    // Wait for PIE to initialize
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));

    // Run the actual test
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        USardineGetNFTCheckoutTokenTestHelper* TestHelper = NewObject<USardineGetNFTCheckoutTokenTestHelper>();
        TestHelper->ParentTest = this;
        TestHelper->RunTest();

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForGetNFTCheckoutToken(TestHelper));
        return true;
    }));
    return true;
}

void USardineGetNFTCheckoutTokenTestHelper::RunTest()
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

bool FWaitForGetNFTCheckoutToken::Update()
{
    return TestHelper->IsTestComplete();
}
