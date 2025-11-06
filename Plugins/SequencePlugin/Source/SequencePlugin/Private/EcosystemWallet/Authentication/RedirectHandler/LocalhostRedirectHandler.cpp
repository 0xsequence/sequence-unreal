#include "LocalhostRedirectHandler.h"
#include "EcosystemWallet/Authentication/LocalhostListener.h"
#include "Engine/Engine.h"
#include "Misc/ScopeExit.h"
#include "Logging/LogMacros.h"

void FLocalhostRedirectHandler::WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
    FPlatformProcess::LaunchURL(*FullUrl, nullptr, nullptr);

    if (ULocalhostListener::GetInstance() != nullptr)
    {
        ULocalhostListener::GetInstance()->WaitForResponse(OnSuccess, OnFailure);
        return;
    }

    OnFailure(FSequenceError(EErrorType::EmptyResponse, TEXT("Could not find ULocalhostListener subsystem")));
}
