#include "SardineGetClientTokenTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Marketplace/Sardine/SardineCheckout.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "Util/Async.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FSardineGetClientTokenTest, "SequencePlugin.EndToEnd.SardineTests.GetClientTokenTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FSardineGetClientTokenTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Sardine Get Client Token Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FSardineGetClientTokenTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting Sardine Get Client Token Test"));
    
    // Start PIE
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    
    // Wait for PIE to initialize
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));

    // Run the actual test
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        USardineGetClientTokenTestHelper* TestHelper = NewObject<USardineGetClientTokenTestHelper>();
        TestHelper->ParentTest = this;
        TestHelper->RunTest();

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForGetClientTokenToComplete(TestHelper));
        return true;
    }));
    return true;
}

void USardineGetClientTokenTestHelper::RunTest()
{
    Checkout = NewObject<USardineCheckout>();

    const TSuccessCallback<FString> GetClientTokenSuccess = [this](const FString& Token)
    {
        bTestComplete = true;
        const FString Message = "SardineGetClientToken request succeeded with token";
        ParentTest->AddInfo(FString::Printf(TEXT("%s: %s"), *Message, *Token));
    };

    const FFailureCallback GetClientTokenFailure = [this](const FSequenceError& Error)
    {
        bTestComplete = true;
        const FString Message = "Request Failure";
        ParentTest->AddError(FString::Printf(TEXT("%s: %s."), *Message, *Error.Message));
    };

    Checkout->SardineGetClientToken(GetClientTokenSuccess, GetClientTokenFailure);
}

bool FWaitForGetClientTokenToComplete::Update()
{
    return TestHelper->IsTestComplete();
}
