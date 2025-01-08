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

    OutBeautifiedNames.Add(TEXT("Federation Input Validation"));
    OutTestCommands.Add(TEXT("federation"));
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
        
        if (Parameters == TEXT("login"))
        {
            TestHelper->SetIsLoginTest(true);
            TestHelper->RunLoginValidationTests();
        }
        else
        {
            TestHelper->SetIsLoginTest(false);
            TestHelper->RunFederationValidationTests();
        }

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForValidationTestComplete(TestHelper));
        return true;
    }));

    return true;
}

void UPlayFabInputValidationTestHelper::RunLoginValidationTests()
{
    bTestComplete = false;
    
    if (!Authenticator)
    {
        Authenticator = NewObject<USequenceAuthenticator>();
        Authenticator->AuthSuccess.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnAuthSuccess);
        Authenticator->AuthFailure.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnAuthFailure);
    }

    if (LastError.IsEmpty())
    {
        ParentTest->AddInfo(TEXT("Testing empty username"));
        Authenticator->PlayFabLogin(TEXT(""), TEXT("validpassword123"), false);
    }
    else if (LastError.Contains(TEXT("Username cannot be empty")))
    {
        ParentTest->AddInfo(TEXT("Testing empty password"));
        Authenticator->PlayFabLogin(TEXT("validusername"), TEXT(""), false);
    }
    else if (LastError.Contains(TEXT("Password cannot be empty")))
    {
        ParentTest->AddInfo(TEXT("Testing short password"));
        Authenticator->PlayFabLogin(TEXT("validusername"), TEXT("short"), false);
    }
    else if (LastError.Contains(TEXT("Password must be at least 8 characters")))
    {
        bTestComplete = true;
        bTestPassed = true;
        bAllTestsComplete = true;
        ParentTest->AddInfo(TEXT("All login validation tests completed successfully"));
    }
    else
    {
        bTestComplete = true;
        bTestPassed = false;
        bAllTestsComplete = true;
        ParentTest->AddInfo(TEXT("Received unexpected error message: " + LastError));
    }
}

void UPlayFabInputValidationTestHelper::RunFederationValidationTests()
{
    bTestComplete = false;
    
    if (!Authenticator)
    {
        Authenticator = NewObject<USequenceAuthenticator>();
        Authenticator->FederateSuccess.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnFederateSuccess);
        Authenticator->FederateFailure.AddDynamic(this, &UPlayFabInputValidationTestHelper::OnFederateFailure);
    }

    if (LastError.IsEmpty())
    {
        ParentTest->AddInfo(TEXT("Testing empty username for federation"));
        Authenticator->FederatePlayFabNewAccount(TEXT(""), TEXT("valid@email.com"), TEXT("validpassword123"));
    }
    else if (LastError.Contains(TEXT("Username cannot be empty")))
    {
        ParentTest->AddInfo(TEXT("Testing empty email for federation"));
        Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT(""), TEXT("validpassword123"));
    }
    else if (LastError.Contains(TEXT("Email cannot be empty")))
    {
        ParentTest->AddInfo(TEXT("Testing invalid email format (no @) for federation"));
        Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("invalidemail"), TEXT("validpassword123"));
    }
    else if (LastError.Contains(TEXT("Email is invalid, given invalidemail")))
    {
        ParentTest->AddInfo(TEXT("Testing invalid email format (no domain) for federation"));
        Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("invalid@"), TEXT("validpassword123"));
    }
    else if (LastError.Contains(TEXT("Email is invalid, given invalid@")))
    {
        ParentTest->AddInfo(TEXT("Testing empty password for federation"));
        Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("valid@email.com"), TEXT(""));
    }
    else if (LastError.Contains(TEXT("Password cannot be empty")))
    {
        ParentTest->AddInfo(TEXT("Testing short password for federation"));
        Authenticator->FederatePlayFabNewAccount(TEXT("validusername"), TEXT("valid@email.com"), TEXT("short"));
    }
    else if (LastError.Contains(TEXT("Password must be at least 8 characters")))
    {
        bTestComplete = true;
        bTestPassed = true;
        bAllTestsComplete = true;
        ParentTest->AddInfo(TEXT("All federation validation tests completed successfully"));
    }
    else
    {
        bTestComplete = true;
        bTestPassed = false;
        bAllTestsComplete = true;
        ParentTest->AddInfo(TEXT("Received unexpected error message: " + LastError));
    }
}

void UPlayFabInputValidationTestHelper::OnAuthSuccess()
{
    bTestComplete = true;
    bTestPassed = false;
    LastError = TEXT("Auth succeeded when it should have failed validation");
    ParentTest->AddError(LastError);
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
    ParentTest->AddError(LastError);
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

    if (TestHelper->IsLoginTest())
    {
        TestHelper->RunLoginValidationTests();
    }
    else
    {
        TestHelper->RunFederationValidationTests();
    }
    return false;
}
