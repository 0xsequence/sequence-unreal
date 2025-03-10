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
    if (!Authenticator)
    {
        Authenticator = NewObject<USequenceAuthenticator>();
        
        if (CurrentValidationType == EValidationType::Login || CurrentValidationType == EValidationType::NewAccount)
        {
            // TODO: Re-add delegates as function parameters
            //Authenticator->AuthSuccess.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnAuthSuccess);
            //Authenticator->AuthFailure.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnAuthFailure);
        }
        else
        {
            //Authenticator->FederateSuccess.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnFederateSuccess);
            //Authenticator->FederateFailure.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnFederateFailure);
        }
    }
}

TArray<FValidationStep> UPlayFabInputValidationTestHelper::GetValidationStepsForType(EValidationType Type)
{
    TArray<FValidationStep> Steps;
    
    switch(Type)
    {
        case EValidationType::Login:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"), 
                    TEXT("Testing empty username"),
                    [this]() { Authenticator->PlayFabLogin(TEXT(""), TEXT("validpassword123"), false); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password"),
                    [this]() { Authenticator->PlayFabLogin(TEXT("validusername"), TEXT(""), false); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password"),
                    [this]() { Authenticator->PlayFabLogin(TEXT("validusername"), TEXT("short"), false); })
            };
            break;

        case EValidationType::NewAccount:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"),
                    TEXT("Testing empty username for new account"),
                    [this]() { Authenticator->PlayFabRegisterAndLogin(TEXT(""), TEXT("valid@email.com"), TEXT("validpassword123"), false); }),
                FValidationStep(TEXT("Email cannot be empty"),
                    TEXT("Testing empty email for new account"),
                    [this]() { Authenticator->PlayFabRegisterAndLogin(TEXT("validusername"), TEXT(""), TEXT("validpassword123"), false); }),
                FValidationStep(TEXT("Email is invalid, given invalidemail"),
                    TEXT("Testing invalid email format for new account"),
                    [this]() { Authenticator->PlayFabRegisterAndLogin(TEXT("validusername"), TEXT("invalidemail"), TEXT("validpassword123"), false); }),
                FValidationStep(TEXT("Email is invalid, given invalid@"),
                    TEXT("Testing another invalid email format"),
                    [this]() { Authenticator->PlayFabRegisterAndLogin(TEXT("validusername"), TEXT("invalid@"), TEXT("validpassword123"), false); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password for new account"),
                    [this]() { Authenticator->PlayFabRegisterAndLogin(TEXT("validusername"), TEXT("valid@email.com"), TEXT(""), false); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password for new account"),
                    [this]() { Authenticator->PlayFabRegisterAndLogin(TEXT("validusername"), TEXT("valid@email.com"), TEXT("short"), false); })
            };
            break;

        case EValidationType::Federation:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"),
                    TEXT("Testing empty username for federation"),
                    [this]() { Authenticator->FederatePlayFabNewAccount(TEXT(""), TEXT("valid@email.com"), TEXT("validpassword123")); }),
                FValidationStep(TEXT("Email cannot be empty"),
                    TEXT("Testing empty email for federation"),
                    [this]() { Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT(""), TEXT("validpassword123")); }),
                FValidationStep(TEXT("Email is invalid, given invalidemail"),
                    TEXT("Testing invalid email format for federation"),
                    [this]() { Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("invalidemail"), TEXT("validpassword123")); }),
                FValidationStep(TEXT("Email is invalid, given invalid@"),
                    TEXT("Testing another invalid email format"),
                    [this]() { Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("invalid@"), TEXT("validpassword123")); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password for federation"),
                    [this]() { Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("valid@email.com"), TEXT("")); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password for federation"),
                    [this]() { Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("valid@email.com"), TEXT("short")); })
            };
            break;

        case EValidationType::FederationLogin:
            Steps = {
                FValidationStep(TEXT("Username cannot be empty"),
                    TEXT("Testing empty username for federation login"),
                    [this]() { Authenticator->FederatePlayFabLogin(TEXT(""), TEXT("validpassword123")); }),
                FValidationStep(TEXT("Password cannot be empty"),
                    TEXT("Testing empty password for federation login"),
                    [this]() { Authenticator->FederatePlayFabLogin(TEXT("validusername"), TEXT("")); }),
                FValidationStep(TEXT("Password must be at least 8 characters"),
                    TEXT("Testing short password for federation login"),
                    [this]() { Authenticator->FederatePlayFabLogin(TEXT("validusername"), TEXT("short")); })
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
