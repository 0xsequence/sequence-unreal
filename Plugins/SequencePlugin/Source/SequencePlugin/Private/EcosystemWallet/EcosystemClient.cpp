#include "EcosystemClient.h"
#include "Authentication/RedirectHandler/BrowserRedirectHandler.h"
#include "Authentication/RedirectHandler/LocalhostRedirectHandler.h"
#include "Primitives/Permission/SessionPermissions.h"
#include "Requests/ConnectArgs.h"
#include "Requests/ConnectResponse.h"
#include "Sequence/SequenceSdk.h"
#include "Signers/SessionCredentials.h"
#include "Storage/SessionStorage.h"
#include "Types/CryptoWallet.h"

UEcosystemClient::UEcosystemClient()
{
    this->Storage = NewObject<USessionStorage>();
    this->Origin = "http://localhost:4444/api"; // Define this for each platform

#if PLATFORM_IOS || PLATFORM_ANDROID || PLATFORM_MAC
    this->RedirectHandler = MakeShared<FLocalhostRedirectHandler>();
#elif PLATFORM_WINDOWS || PLATFORM_MAC
    this->RedirectHandler = MakeShared<FBrowserRedirectHandler>();
#else
    this->RedirectHandler = MakeShared<FLocalhostRedirectHandler>();
#endif
}

void UEcosystemClient::CreateNewSession(ESessionCreationType Type, const FString& PreferredLoginMethod, const FString& Email,
		FSessionPermissions Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
    const FString ChainIdStr = SequenceSdk::GetChainIdString();
    UCryptoWallet* SessionWallet = UCryptoWallet::Make();
    FString WalletAddress = SessionWallet->GetWalletAddress().ToHexWithPrefix();

    const bool bIncludeImplicitSession = (Type == ESessionCreationType::IncludeImplicit);

    FConnectArgs Payload;
    Payload.SessionAddress           = WalletAddress;
    Payload.PreferredLoginMethod     = PreferredLoginMethod;
    Payload.Email                    = Email;
    Payload.Origin                   = this->Origin;
    Payload.Permissions              = Permissions;
    Payload.bIncludeImplicitSession  = bIncludeImplicitSession;

    const FString& PayloadJson = USequenceSupport::StructToString(Payload);
    UE_LOG(LogTemp, Display, TEXT("Payload: %s"), *PayloadJson);
    
    const FString Action = (Type == ESessionCreationType::AddExplicit)
                               ? TEXT("addExplicitSession")
                               : TEXT("createNewSession");

    const TSuccessCallback<FConnectResponseData> OnHandlerSuccess = [this, SessionWallet, OnSuccess](FConnectResponseData Response)
    {
        FSessionCredentials Credentials;
        Credentials.Address = Response.WalletAddress;
        Credentials.SessionAddress = SessionWallet->GetWalletAddress().ToHexWithPrefix();
        Credentials.PrivateKey = SessionWallet->GetWalletPrivateKeyString();
        Credentials.LoginMethod = Response.LoginMethod;
        Credentials.Email = Response.Email;

        this->Storage->AddSession(Credentials);
        
        UE_LOG(LogTemp, Display, TEXT("Wallet Address %s"), *Response.WalletAddress);
        OnSuccess(true);
    };

    this->SendRequest<FConnectArgs, FConnectResponseData>("connect", Action, Payload, OnHandlerSuccess, OnFailure);
}
