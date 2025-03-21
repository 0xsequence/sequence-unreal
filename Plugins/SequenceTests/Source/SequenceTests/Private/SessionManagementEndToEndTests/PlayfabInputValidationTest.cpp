#include "PlayfabInputValidationTest.h"
#include "Engine/World.h"
#include "Misc/OutputDeviceNull.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FPlayFabInputValidationTests, "SequencePlugin.EndToEnd.SessionManagement.PlayFabInputValidation",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

void FPlayFabInputValidationTests::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Login Input Validation"));
    OutTestCommands.Add(TEXT("login"));

    OutBeautifiedNames.Add(TEXT("New Account Input Validation"));
    OutTestCommands.Add(TEXT("newaccount"));

    OutBeautifiedNames.Add(TEXT("Federation Input Validation"));
    OutTestCommands.Add(TEXT("federation"));

    OutBeautifiedNames.Add(TEXT("Federation Login Input Validation"));
    OutTestCommands.Add(TEXT("federation_login"));
}

bool FPlayFabInputValidationTests::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting PlayFab Input Validation Test"));
    
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, Parameters]()
    {
        UPlayFabInputValidationTestHelper* TestHelper = NewObject<UPlayFabInputValidationTestHelper>();
        TestHelper->ParentTest = this;
        
        EValidationType Type;
        if (Parameters == TEXT("login"))
            Type = EValidationType::Login;
        else if (Parameters == TEXT("newaccount"))
            Type = EValidationType::NewAccount;
        else if (Parameters == TEXT("federation"))
            Type = EValidationType::Federation;
        else
            Type = EValidationType::FederationLogin;

        TestHelper->SetValidationType(Type);
        TestHelper->RunValidationTests(Type);

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForValidationTestComplete(TestHelper));
        return true;
    }));

    return true;
}

void UPlayFabInputValidationTestHelper::InitializeAuthenticator()
{
    if (!Sessions)
    {
        Sessions = NewObject<USequenceSessions>();
    }
}

TArray<FValidationStep> UPlayFabInputValidationTestHelper::GetValidationStepsForType(EValidationType Type)
{
    const TFunction<void()> OnSuccess = [this]()
    {
        this->OnAuthSuccess();
    };

    const FFailureCallback OnFailure = [this](const FSequenceError& Error)
    {
        this->OnAuthFailure(Error.Message);
    };

    const TFunction<void (FFederationSupportData)> OnFederationRequired = [this](const FFederationSupportData& FederationData)
    {
        UE_LOG(LogEditorAutomationTests, Warning, TEXT("Federation required."));
    };
    
    TArray<FValidationStep> Steps;
    
    switch(Type)
    {
        case EValidationType::Login:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"), 
                    TEXT("Testing empty username"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabLogin(TEXT(""), TEXT("validpassword123"), OnSuccess, OnFailure, OnFederationRequired); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabLogin(TEXT("validusername"), TEXT(""), OnSuccess, OnFailure, OnFederationRequired); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabLogin(TEXT("validusername"), TEXT("short"), OnSuccess, OnFailure, OnFederationRequired); })
            };
            break;

        case EValidationType::NewAccount:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"),
                    TEXT("Testing empty username for new account"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabRegistration(TEXT(""), TEXT("valid@email.com"), TEXT("validpassword123"), OnSuccess, OnFailure, OnFederationRequired); }),
                FValidationStep(TEXT("Email cannot be empty"),
                    TEXT("Testing empty email for new account"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabRegistration(TEXT("validusername"), TEXT(""), TEXT("validpassword123"), OnSuccess, OnFailure, OnFederationRequired); }),
                FValidationStep(TEXT("Email is invalid, given invalidemail"),
                    TEXT("Testing invalid email format for new account"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabRegistration(TEXT("validusername"), TEXT("invalidemail"), TEXT("validpassword123"), OnSuccess, OnFailure, OnFederationRequired); }),
                FValidationStep(TEXT("Email is invalid, given invalid@"),
                    TEXT("Testing another invalid email format"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabRegistration(TEXT("validusername"), TEXT("invalid@"), TEXT("validpassword123"), OnSuccess, OnFailure, OnFederationRequired); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password for new account"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabRegistration(TEXT("validusername"), TEXT("valid@email.com"), TEXT(""), OnSuccess, OnFailure, OnFederationRequired); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password for new account"),
                    [this, OnSuccess, OnFailure, OnFederationRequired]() { Sessions->PlayFabRegistration(TEXT("validusername"), TEXT("valid@email.com"), TEXT("short"), OnSuccess, OnFailure, OnFederationRequired); })
            };
            break;

        case EValidationType::Federation:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"),
                    TEXT("Testing empty username for federation"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabRegistration(TEXT(""), TEXT("valid@email.com"), TEXT("validpassword123"), OnSuccess, OnFailure); }),
                FValidationStep(TEXT("Email cannot be empty"),
                    TEXT("Testing empty email for federation"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabRegistration(TEXT("validusername"), TEXT(""), TEXT("validpassword123"), OnSuccess, OnFailure); }),
                FValidationStep(TEXT("Email is invalid, given invalidemail"),
                    TEXT("Testing invalid email format for federation"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabRegistration(TEXT("validusername"), TEXT("invalidemail"), TEXT("validpassword123"), OnSuccess, OnFailure); }),
                FValidationStep(TEXT("Email is invalid, given invalid@"),
                    TEXT("Testing another invalid email format"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabRegistration(TEXT("validusername"), TEXT("invalid@"), TEXT("validpassword123"), OnSuccess, OnFailure); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password for federation"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabRegistration(TEXT("validusername"), TEXT("valid@email.com"), TEXT(""), OnSuccess, OnFailure); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password for federation"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabRegistration(TEXT("validusername"), TEXT("valid@email.com"), TEXT("short"), OnSuccess, OnFailure); })
            };
            break;

        case EValidationType::FederationLogin:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"),
                    TEXT("Testing empty username for federation login"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabLogin(TEXT(""), TEXT("validpassword123"), OnSuccess, OnFailure); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password for federation login"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabLogin(TEXT("validusername"), TEXT(""), OnSuccess, OnFailure); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password for federation login"),
                    [this, OnSuccess, OnFailure]() { Sessions->FederatePlayFabLogin(TEXT("validusername"), TEXT("short"), OnSuccess, OnFailure); })
            };
            break;
    }
    
    return Steps;
}

void UPlayFabInputValidationTestHelper::RunValidationTests(EValidationType ValidationType)
{
    InitializeAuthenticator();
    bTestComplete = false;
    
    TArray<FValidationStep> Steps = GetValidationStepsForType(ValidationType);
    
    if (LastError.IsEmpty())
    {
        CurrentStepIndex = 0;
        const FValidationStep& CurrentStep = Steps[CurrentStepIndex];
        ParentTest->AddInfo(CurrentStep.TestDescription);
        CurrentStep.TestAction();
    }
    
    if (CurrentStepIndex >= 0 && CurrentStepIndex < Steps.Num())
    {
        const FValidationStep& CurrentStep = Steps[CurrentStepIndex];
        if (!LastError.Contains(CurrentStep.ExpectedError))
        {
            ParentTest->AddError(FString::Printf(TEXT("Unexpected error received: %s. Expected: %s"), 
                *LastError, *Steps[CurrentStepIndex].ExpectedError));
            bTestComplete = true;
            bTestPassed = false;
            bAllTestsComplete = true;
            return;
        }
    }
    
    CurrentStepIndex++;

    if (CurrentStepIndex >= Steps.Num())
    {
        bTestComplete = true;
        bTestPassed = true;
        bAllTestsComplete = true;
        ParentTest->AddInfo(TEXT("All validation tests completed successfully"));
        return;
    }

    const FValidationStep& CurrentStep = Steps[CurrentStepIndex];
    ParentTest->AddInfo(CurrentStep.TestDescription);
    CurrentStep.TestAction();
}

void UPlayFabInputValidationTestHelper::OnAuthSuccess()
{
    bTestComplete = true;
    bTestPassed = false;
    LastError = TEXT("Auth succeeded when it should have failed validation");
    ParentTest->AddInfo(TEXT("Unexpected success: Auth succeeded when it should have failed validation"));
}

void UPlayFabInputValidationTestHelper::OnAuthFailure(const FString& Error)
{
    bTestComplete = true;
    bTestPassed = true;
    LastError = Error;
    ParentTest->AddInfo(FString::Printf(TEXT("Auth failed as expected with error: %s"), *Error));
}

void UPlayFabInputValidationTestHelper::OnFederateSuccess()
{
    bTestComplete = true;
    bTestPassed = false;
    LastError = TEXT("Federation succeeded when it should have failed validation");
    ParentTest->AddInfo(TEXT("Unexpected success: Federation succeeded when it should have failed validation"));
}

void UPlayFabInputValidationTestHelper::OnFederateFailure(const FString& Error)
{
    bTestComplete = true;
    bTestPassed = true;
    LastError = Error;
    ParentTest->AddInfo(FString::Printf(TEXT("Federation failed as expected with error: %s"), *Error));
}

bool FWaitForValidationTestComplete::Update()
{
    if (!TestHelper->IsTestComplete())
    {
        return false;
    }

    if (!TestHelper->GetTestPassed() || TestHelper->AreAllTestsComplete())
    {
        if (TestHelper->GetTestPassed())
        {
            TestHelper->ParentTest->TestTrue(TEXT("All validation tests completed successfully"), true);
        }
        return true;
    }

    TestHelper->RunValidationTests(TestHelper->GetValidationType());
    return false;
}
