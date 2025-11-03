#include "EcosystemClient.h"
#include "Authentication/RedirectHandler/BrowserRedirectHandler.h"
#include "Authentication/RedirectHandler/LocalhostRedirectHandler.h"
#include "Authentication/RedirectHandler/MobileRedirectHandler.h"
#include "EcosystemWallet/Permissions/Permissions.h"
#include "EcosystemWallet/Primitives/Permission/SessionPermissions.h"
#include "Requests/ConnectArgs.h"
#include "Requests/SessionArgs.h"
#include "Sequence/SequenceSdk.h"
#include "Signers/SessionCredentials.h"
#include "Types/CryptoWallet.h"

UEcosystemClient::UEcosystemClient()
{
    this->Origin = "http://localhost:4444/api"; // Define this for each platform

#if PLATFORM_IOS || PLATFORM_ANDROID
    this->RedirectHandler = MakeShared<FMobileRedirectHandler>();
#else
    this->RedirectHandler = MakeShared<FLocalhostRedirectHandler>();
#endif

    this->RedirectHandler->SetRedirectUrl(this->Origin);
}

void UEcosystemClient::CreateNewSession(ESessionCreationType Type, const FString& PreferredLoginMethod, const FString& Email,
		const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
    const FString ChainIdStr = SequenceSdk::GetChainIdString();
    UCryptoWallet* SessionWallet = UCryptoWallet::Make();
    FString SessionAddress = SessionWallet->GetWalletAddress().ToHexWithPrefix();

    FSessionPermissions SessionPermissions;
    if (Permissions.GetObject() == nullptr)
    {
        SessionPermissions = FSessionPermissions();
    }
    else
    {
        SessionPermissions = Permissions->GetSessionPermissions();
    }

    SessionPermissions.SessionAddress = SessionAddress;

    const bool IsImplicit = SessionPermissions.Permissions.Num() == 0;

    FSessionArgs SessionArgs;
    if (IsImplicit)
    {
        SessionArgs = FSessionArgs(SessionAddress);
    }
    else
    {
        SessionArgs = FSessionArgs(
            SessionPermissions.SessionAddress,
            SessionPermissions.ChainId,
            SessionPermissions.ValueLimit,
            SessionPermissions.Deadline,
            SessionPermissions.Permissions);
    }

    const bool IncludeImplicitSession = (Type == ESessionCreationType::IncludeImplicit || IsImplicit);

    FConnectArgs Payload;
    Payload.PreferredLoginMethod     = PreferredLoginMethod;
    Payload.Email                    = Email;
    Payload.Origin                   = this->Origin.Replace(TEXT("/api"), TEXT(""));
    Payload.Session                 = SessionArgs;
    Payload.IncludeImplicitSession  = IncludeImplicitSession;

    const FString& PayloadJson = USequenceSupport::StructToString(Payload);
    UE_LOG(LogTemp, Display, TEXT("Payload: %s"), *PayloadJson);
    
    const FString Action = (Type == ESessionCreationType::AddExplicit)
                               ? TEXT("addExplicitSession")
                               : TEXT("createNewSession");

    const TFunction<void(FSessionCredentials)> OnHandlerSuccess = [this, ChainIdStr, SessionWallet, IsImplicit, OnSuccess](FSessionCredentials Credentials)
    {
        Credentials.IsExplicit = !IsImplicit;
        Credentials.PrivateKey = SessionWallet->GetWalletPrivateKeyString();
        Credentials.SessionAddress = SessionWallet->GetWalletAddress().ToHexWithPrefix();
        Credentials.ChainId = ChainIdStr;

        FSessionStorage::AddSession(Credentials);
        
        OnSuccess(true);
    };

    this->SendRequest<FConnectArgs, FSessionCredentials>("connect", Action, Payload, OnHandlerSuccess, OnFailure);
}
