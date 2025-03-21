#include "FederateAuthTest.h"
#include "Engine/World.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FWaaSFederatedAuthTests, "SequencePlugin.EndToEnd.SessionManagement.FederateAuthTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

void FWaaSFederatedAuthTests::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Test Account Association"));
    OutTestCommands.Add(TEXT(""));
}

bool FWaaSFederatedAuthTests::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting Federated Auth Test"));
    
    // Start PIE
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    
    // Wait for PIE to initialize
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    
    // Run the actual test
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        UWaaSFederatedAuthTestsHelper* TestHelper = NewObject<UWaaSFederatedAuthTestsHelper>();
        TestHelper->ParentTest = this;
        TestHelper->RunTest();

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForFederationToComplete(TestHelper));
        return true;
    }));

    return true;
}

void UWaaSFederatedAuthTestsHelper::RunTest()
{
    Sessions = NewObject<USequenceSessions>();
    Email = FString::Printf(TEXT("test-%lld@example.com"), FDateTime::UtcNow().ToUnixTimestamp());
    Username = FString::Printf(TEXT("testuser%lld"), FDateTime::UtcNow().ToUnixTimestamp());
    Password = "testpassword";

    const TFunction<void()> OnSuccess = [this]()
    {
        this->OnAuthSuccess();
    };

    const FFailureCallback OnFailure = [this](const FSequenceError& Error)
    {
        this->OnAuthFailure(Error.Message);
    };
    
    Sessions->StartGuestSession(OnSuccess, OnFailure);
}

void UWaaSFederatedAuthTestsHelper::OnFederateSuccess()
{
    bTestComplete = true;
    ParentTest->AddInfo(TEXT("Successfully federated auth"));
}

void UWaaSFederatedAuthTestsHelper::OnFederateFailure(const FString& Error)
{
    bTestComplete = true;
    ParentTest->AddError(FString::Printf(TEXT("Federation failed: %s"), *Error));
}

void UWaaSFederatedAuthTestsHelper::OnAuthSuccess()
{
    const TFunction<void()> OnSuccess = [this]()
    {
        this->OnFederateSuccess();
    };

    const FFailureCallback OnFailure = [this](const FSequenceError& Error)
    {
        this->OnFederateFailure(Error.Message);
    };
    
    Sessions->FederatePlayFabRegistration(Username, Email, Password, OnSuccess, OnFailure);
}

void UWaaSFederatedAuthTestsHelper::OnAuthFailure(const FString& Error)
{
    bTestComplete = true;
    ParentTest->AddError(FString::Printf(TEXT("Login failed: %s"), *Error));
}

bool FWaitForFederationToComplete::Update()
{
    return TestHelper->IsTestComplete();
}