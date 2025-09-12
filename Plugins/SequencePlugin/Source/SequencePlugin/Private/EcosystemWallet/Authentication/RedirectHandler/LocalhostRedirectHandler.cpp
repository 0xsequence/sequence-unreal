#include "LocalhostRedirectHandler.h"
#include "EcosystemWallet/Authentication/LocalhostListener.h"
#include "Engine/Engine.h"
#include "Misc/ScopeExit.h"
#include "Logging/LogMacros.h"

void FLocalhostRedirectHandler::WaitForResponseImpl(const FString& FullUrl, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
    FPlatformProcess::LaunchURL(*FullUrl, nullptr, nullptr);

    ULocalhostListener* Listener = ULocalhostListener::GetInstance();
    Listener->WaitForResponse(OnSuccess, OnFailure);
}
