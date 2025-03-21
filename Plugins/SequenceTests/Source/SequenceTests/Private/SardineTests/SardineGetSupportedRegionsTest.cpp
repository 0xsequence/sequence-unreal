#include "SardineGetSupportedRegionsTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Marketplace/Sardine/SardineCheckout.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"
#include "Util/Async.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FSardineGetSupportedRegionsTest, "SequencePlugin.EndToEnd.SardineTests.GetSupportedRegionsTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void FSardineGetSupportedRegionsTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Sardine Get Supported Regions Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FSardineGetSupportedRegionsTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting Sardine Get Supported Regions Test"));
    
    // Start PIE
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    
    // Wait for PIE to initialize
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));

    // Run the actual test
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        USardineGetSupportedRegionsTestHelper* TestHelper = NewObject<USardineGetSupportedRegionsTestHelper>();
        TestHelper->ParentTest = this;
        TestHelper->RunTest();

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForGetSupportedRegionsToComplete(TestHelper));
        return true;
    }));
    return true;
}

void USardineGetSupportedRegionsTestHelper::RunTest()
{
    Checkout = NewObject<USardineCheckout>();

    const TSuccessCallback<TArray<FSardineRegion>> GetSupportedRegionsSuccess = [this](const TArray<FSardineRegion>& Regions)
    {
        bTestComplete = true;
        const FString Message = "SardineGetSupportedRegions request succeeded with regions:";
        ParentTest->AddInfo(Message);
        for (const FSardineRegion& Region : Regions)
        {
            ParentTest->AddInfo(FString::Printf(TEXT("%s: %s."), *Region.CountryCode, *Region.Name));
        }
    };

    const FFailureCallback GenericFailure = [this](const FSequenceError& Error)
    {
        bTestComplete = true;
        const FString Message = "Request Failure";
        ParentTest->AddError(FString::Printf(TEXT("%s: %s."), *Message, *Error.Message));
    };

    Checkout->SardineGetSupportedRegions(GetSupportedRegionsSuccess, GenericFailure);
}

bool FWaitForGetSupportedRegionsToComplete::Update()
{
    return TestHelper->IsTestComplete();
}
