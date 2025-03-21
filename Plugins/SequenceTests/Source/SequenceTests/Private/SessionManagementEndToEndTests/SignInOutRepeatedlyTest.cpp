#include "SignInOutRepeatedlyTest.h"
#include "Engine/World.h"
#include "Sequence/SequenceWallet.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FSignInOutRepeatedlyTest, "SequencePlugin.EndToEnd.SessionManagement.SignInOutRepeatedlyTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::ClientContext)

void FSignInOutRepeatedlyTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(TEXT("Sign In and Out Repeatedly Test"));
    OutTestCommands.Add(TEXT(""));
}

bool FSignInOutRepeatedlyTest::RunTest(const FString& Parameters)
{
    AddInfo(TEXT("Starting Sign In and Out Repeatedly Test"));
    
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(true));
    ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(1.0f));
    
    ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this]()
    {
        USignInOutRepeatedlyTestHelper* TestHelper = NewObject<USignInOutRepeatedlyTestHelper>();
        TestHelper->ParentTest = this;
        TestHelper->RunTest();

        ADD_LATENT_AUTOMATION_COMMAND(FWaitForSignInOutCompletion(TestHelper));
        return true;
    }));

    return true;
}

void USignInOutRepeatedlyTestHelper::RunTest()
{
    Sessions = NewObject<USequenceSessions>();
    if (!Sessions)
    {
        ParentTest->AddError(TEXT("Error creating authenticator"));
        bTestComplete = true;
        return;
    }
    
    ConnectAsGuest();
}

void USignInOutRepeatedlyTestHelper::ConnectAsGuest()
{
    const TFunction<void()> OnApiSuccess = [this]()
    {
        this->OnAuthSuccess();
    };

    const FFailureCallback OnApiFailure = [this](const FSequenceError& Error)
    {
        this->OnAuthFailure(Error.Message);
    };

    this->Sessions->StartGuestSession(OnApiSuccess, OnApiFailure);
}

void USignInOutRepeatedlyTestHelper::OnAuthSuccess()
{
    if (Repetitions < MaxRepetitions)
    {
        Repetitions++;
        const USequenceWallet* Wallet = NewObject<USequenceWallet>();
        TSet<FString> UniqueSessionIds(SessionIds);
        bool bAllSessionIdsUnique = UniqueSessionIds.Num() == SessionIds.Num();
        if (!bAllSessionIdsUnique)
        {
            bTestComplete = true;
            ParentTest->AddError(TEXT("Session Ids are not unique"));
            return;
        }
            
        SessionIds.Add(Wallet->GetSessionId());
        Wallet->SignOut();
        ConnectAsGuest();
    }
    else
    {
        bTestComplete = true;
        ParentTest->AddInfo(FString::Printf(TEXT("Successfully signed in and out %d times"), MaxRepetitions));
    }
}

void USignInOutRepeatedlyTestHelper::OnAuthFailure(const FString& Error)
{
    bTestComplete = true;
    ParentTest->AddError(FString::Printf(TEXT("Authentication failed: %s"), *Error));
}

bool FWaitForSignInOutCompletion::Update()
{
    return TestHelper->IsTestComplete();
}