#include "SardineGetSupportedFiatCurrenciesTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Marketplace/Sardine/SardineCheckout.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "Util/Async.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FSardineGetSupportedFiatCurrenciesTest, "SequencePlugin.EndToEnd.SardineTests.GetSupportedFiatCurrenciesTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FSardineGetSupportedFiatCurrenciesTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Sardine Get Supported Fiat Currencies Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FSardineGetSupportedFiatCurrenciesTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting Sardine Get Supported Fiat Currencies Test"));
    
    // Start PIE
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    
    // Wait for PIE to initialize
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));

    // Run the actual test
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        USardineGetSupportedFiatCurrenciesTestHelper* TestHelper = NewObject<USardineGetSupportedFiatCurrenciesTestHelper>();
        TestHelper->ParentTest = this;
        TestHelper->RunTest();

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForGetSupportedFiatCurrenciesToComplete(TestHelper));
        return true;
    }));
    return true;
}

void USardineGetSupportedFiatCurrenciesTestHelper::RunTest()
{
    Checkout = NewObject<USardineCheckout>();

    const TSuccessCallback<TArray<FSardineFiatCurrency>> GetSupportedFiatCurrenciesSuccess = [this](const TArray<FSardineFiatCurrency>& Currencies)
    {
        bTestComplete = true;
        const FString Message = "SardineGetSupportedFiatCurrencies request succeeded with currencies:";
        ParentTest->AddInfo(Message);
        for (const FSardineFiatCurrency& Currency : Currencies)
        {
            ParentTest->AddInfo(FString::Printf(TEXT("%s: %s."), *Currency.CurrencyCode, *Currency.Name));
        }
    };

    const FFailureCallback GenericFailure = [this](const FSequenceError& Error)
    {
        bTestComplete = true;
        const FString Message = "Request Failure";
        ParentTest->AddError(FString::Printf(TEXT("%s: %s."), *Message, *Error.Message));
    };

    Checkout->SardineGetSupportedFiatCurrencies(GetSupportedFiatCurrenciesSuccess, GenericFailure);
}

bool FWaitForGetSupportedFiatCurrenciesToComplete::Update()
{
    return TestHelper->IsTestComplete();
}
