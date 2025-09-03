#include "EcosystemClient.h"
#include "Authentication/RedirectHandler/IRedirectHandler.h"
#include "Authentication/RedirectHandler/RedirectFactory.h"
#include "Primitives/Permission/SessionPermissions.h"
#include "Requests/ConnectArgs.h"
#include "Requests/ConnectResponse.h"
#include "Signers/SessionCredentials.h"
#include "Storage/SessionStorage.h"
#include "Types/CryptoWallet.h"

UEcosystemClient::UEcosystemClient()
{
    this->Storage = NewObject<USessionStorage>();
}

void UEcosystemClient::CreateNewSession(
    ESessionCreationType Type,
    const FString& PreferredLoginMethod,
    const FString& Email,
    FSessionPermissions Permissions,
    const TSuccessCallback<bool>& OnSuccess,
    const FFailureCallback& OnFailure)
{
    const FString ChainIdStr = "421614"; // Arbitrum Sepolia
    UCryptoWallet* SessionWallet = UCryptoWallet::Make();
    FString WalletAddress = SessionWallet->GetWalletAddress().ToHexWithPrefix();

    const FString Origin = "http://localhost:4444/api";
    const bool bIncludeImplicitSession = (Type == ESessionCreationType::IncludeImplicit);

    FConnectArgs Payload;
    Payload.SessionAddress           = WalletAddress;
    Payload.PreferredLoginMethod     = PreferredLoginMethod;
    Payload.Email                    = Email;
    Payload.Origin                   = Origin;
    Payload.Permissions              = Permissions;
    Payload.bIncludeImplicitSession  = bIncludeImplicitSession;

    const FString& PayloadJson = USequenceSupport::StructToString(Payload);
    UE_LOG(LogTemp, Display, TEXT("Payload: %s"), *PayloadJson);
    
    const FString Action = (Type == ESessionCreationType::AddExplicit)
                               ? TEXT("addExplicitSession")
                               : TEXT("createNewSession");
    
    const FString EcosystemUrl = "https://v3.sequence-dev.app";
    const FString Url = FString::Printf(TEXT("%s/request/connect"), *EcosystemUrl);

    const TSharedPtr<IRedirectHandler> HandlerPtr = FRedirectFactory::CreateHandler();
    IRedirectHandler* Handler = HandlerPtr.Get();
    Handler->SetRedirectUrl(Origin);

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

    Handler->WaitForResponse<FConnectArgs, FConnectResponseData>(Url, Action, Payload, OnHandlerSuccess, OnFailure);
}
