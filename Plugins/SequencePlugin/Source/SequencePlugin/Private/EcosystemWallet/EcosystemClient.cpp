#include "EcosystemClient.h"
#include "Authentication/RedirectHandler/IRedirectHandler.h"
#include "Authentication/RedirectHandler/RedirectFactory.h"
#include "Primitives/Permission/SessionPermissions.h"
#include "Requests/ConnectArgs.h"
#include "Requests/ConnectResponse.h"
#include "Types/CryptoWallet.h"

void UEcosystemClient::CreateNewSession(
    ESessionCreationType Type,
    const FString& PreferredLoginMethod,
    const TOptional<FString>& Email,
    const TOptional<FSessionPermissions>& Permissions,
    const TSuccessCallback<bool>& OnSuccess,
    const FFailureCallback& OnFailure)
{
    const FString ChainIdStr = "421614"; // Arbitrum Sepolia
    UCryptoWallet* SessionWallet = UCryptoWallet::Make();

    const FString Origin = "http://localhost:4444/";
    const bool bIncludeImplicitSession = (Type == ESessionCreationType::IncludeImplicit);

    FConnectArgs Payload;
    Payload.SessionAddress           = SessionWallet->GetWalletAddress();
    Payload.PreferredLoginMethod     = PreferredLoginMethod;
    Payload.Email                    = Email;
    Payload.Origin                   = Origin;
    Payload.Permissions              = Permissions;
    Payload.bIncludeImplicitSession  = bIncludeImplicitSession;

    const FString Action = (Type == ESessionCreationType::AddExplicit)
                               ? TEXT("addExplicitSession")
                               : TEXT("createNewSession");
    
    const FString EcosystemUrl = "https://v3.sequence-dev.app";
    const FString Url = FString::Printf(TEXT("%s/request/connect"), *EcosystemUrl);

    const TSharedPtr<IRedirectHandler> HandlerPtr = FRedirectFactory::CreateHandler();
    IRedirectHandler* Handler = HandlerPtr.Get();
    Handler->SetRedirectUrl(Origin);

    const TOptional<FConnectResponse> Response =
            Handler->WaitForResponse<FConnectArgs, FConnectResponse>(Url, Action, Payload);
    
    if (!Response.IsSet())
    {
        throw std::runtime_error("Error during request");
    }

    //const bool bImplicitWithPermissions = bIncludeImplicitSession && Permissions.IsSet();

    /*const int32 CredentialsLen = bImplicitWithPermissions ? 2 : 1;
    TArray<FSessionCredentials> Credentials;
    Credentials.Reserve(CredentialsLen);

    // Primary (explicit if not including implicit)
    Credentials.Emplace(
        !bIncludeImplicitSession,
        SessionWallet.GetPrivateKeyAsHex(),
        Response.Data.WalletAddress,
        Response.Data.Attestation,
        Response.Data.Signature,
        static_cast<int32>(Ecosystem),
        ChainIdStr,
        Response.Data.LoginMethod,
        Response.Data.Email);

    // Optional implicit-with-permissions credential
    if (bImplicitWithPermissions)
    {
        Credentials.Emplace(
            true,
            SessionWallet.GetPrivateKeyAsHex(),
            Response.Data.WalletAddress,
            TOptional<FString>(), // Attestation
            TOptional<FString>(), // Signature
            static_cast<int32>(Ecosystem),
            ChainIdStr,
            Response.Data.LoginMethod,
            Response.Data.Email);
    }

    // Build signers & persist sessions
    TArray<FSessionSigner> Signers;
    Signers.Reserve(Credentials.Num());

    for (const FSessionCredentials& Cred : Credentials)
    {
        Signers.Emplace(Cred);
        SessionStorage::AddSession(Cred);
    }

    return Signers;*/
}
