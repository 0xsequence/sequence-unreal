// Fill out your copyright notice in the Description page of Project Settings.

#include "EcosystemClient.h"

#include "Requests/ConnectArgs.h"

void UEcosystemClient::CreateNewSession(
    ESessionCreationType Type,
    const TOptional<FSessionPermissions>& Permissions,
    const FString& PreferredLoginMethod,
    const TOptional<FString>& Email)
{
    // chainId
    const FString ChainIdStr = ToChainIdString(Permissions);

    // temp EOA wallet for the session
    FEOAWallet SessionWallet;

    // origin & implicit flag
    const FString Origin = RedirectOrigin::GetOriginString();
    const bool bIncludeImplicitSession = (Type == ESessionCreationType::IncludeImplicit);

    // build payload
    FConnectArgs Payload;
    Payload.SessionAddress           = SessionWallet.GetAddress();
    Payload.PreferredLoginMethod     = PreferredLoginMethod;
    Payload.Email                    = Email;
    Payload.Origin                   = Origin;
    Payload.bIncludeImplicitSession  = bIncludeImplicitSession;
    Payload.Permissions              = Permissions;

    // endpoint/action
    const FString Action = (Type == ESessionCreationType::AddExplicit)
                               ? TEXT("addExplicitSession")
                               : TEXT("createNewSession");
    const FString EcosystemUrl = EcosystemBindings::GetUrl(Ecosystem);
    const FString Url = FString::Printf(TEXT("%s/request/connect"), *EcosystemUrl);

    // redirect handler
    TSharedRef<IRedirectHandler> Handler = RedirectFactory::CreateHandler();
    Handler->SetRedirectUrl(Origin);

    // request/response (blocking here to mirror `await`)
    const FConnectResponse Response =
        Handler->WaitForResponse<FConnectArgs, FConnectResponse>(Url, Action, Payload);

    if (!Response.Result)
    {
        throw std::runtime_error("Error during request");
    }

    const bool bImplicitWithPermissions = bIncludeImplicitSession && Permissions.IsSet();

    const int32 CredentialsLen = bImplicitWithPermissions ? 2 : 1;
    TArray<FSessionCredentials> Credentials;
    Credentials.Reserve(CredentialsLen);

    // Primary (explicit if not including implicit)
    Credentials.Emplace(
        /*bIsImplicit*/ !bIncludeImplicitSession,
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
            /*bIsImplicit*/ true,
            SessionWallet.GetPrivateKeyAsHex(),
            Response.Data.WalletAddress,
            /*Attestation*/ TOptional<FString>(),
            /*Signature*/   TOptional<FString>(),
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

    return Signers;
}
