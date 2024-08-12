// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Util/Structs/BE_Enums.h"
#include "Types/Wallet.h"
#include "Dom/JsonObject.h"
#include "ConfigFetcher.h"
#include "NativeEncryptors/GenericNativeEncryptor.h"
#include "Credentials.h"
#include "Util/Async.h"
#include "Authenticator.generated.h"

class USequenceRPCManager;

USTRUCT()
struct SEQUENCEPLUGIN_API FSSOCredentials
{
	GENERATED_USTRUCT_BODY()
	FString URL = "";
	FString ClientID = "";
	FSSOCredentials(){}
	FSSOCredentials(const FString& URLIn, const FString& ClientIDIn)
	{
		URL = URLIn;
		ClientID = ClientIDIn;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthRequiresCode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthFailure);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthSuccess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFederateSuccess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFederateFailure, const FString&, Error);

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuthenticator : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FOnAuthRequiresCode AuthRequiresCode;
	UPROPERTY()
	FOnAuthFailure AuthFailure;
	UPROPERTY()
	FOnAuthSuccess AuthSuccess;
	UPROPERTY()
	FOnFederateSuccess FederateSuccess;
	UPROPERTY()
	FOnFederateFailure FederateFailure;
	
private://Broadcast handlers
	void CallAuthRequiresCode() const;
	void CallAuthFailure() const;
	void CallAuthSuccess() const;
	void CallFederateSuccess() const;
	void CallFederateFailure(const FString& ErrorMessageIn) const;
//vars
private:
	UPROPERTY()
	UGenericNativeEncryptor * Encryptor = nullptr;
	
	const FString SaveSlot = "Cr";
	const uint32 UserIndex = 0;
	FString StateToken = "";

	/**
	 * Used to change behaviour
	 * if the user is federating accounts
	 * OR
	 * if the user is logging in normally
	 */
	bool IsFederating = false;
	
	TMap<ESocialSigninType, FSSOCredentials> SSOProviderMap ={
		{ESocialSigninType::Google,FSSOCredentials(GoogleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID))},
		{ESocialSigninType::Apple,FSSOCredentials(AppleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::AppleClientID))},
		{ESocialSigninType::Discord,FSSOCredentials(DiscordAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::DiscordClientID))},
		{ESocialSigninType::FaceBook,FSSOCredentials(FacebookAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::FacebookClientID))}};
	
	UPROPERTY()
	UWallet* SessionWallet;
	UPROPERTY()
	USequenceRPCManager * SequenceRPCManager = nullptr;
	UPROPERTY()
	bool PurgeCache = true;

	//Static Config variables
	inline static FString UrlScheme = "powered-by-sequence";
	inline static FString GoogleAuthURL = "https://accounts.google.com/o/oauth2/auth";
	inline static FString FacebookAuthURL = "https://www.facebook.com/v18.0/dialog/oauth";
	inline static FString DiscordAuthURL = "https://discord.com/api/oauth2/authorize";
	inline static FString AppleAuthURL = "https://appleid.apple.com/auth/authorize";
	inline static FString RedirectPrefixTrailer = "oauth/callback";
	
private:
	UAuthenticator();

	void InitiateMobleSSO_Internal(const ESocialSigninType& Type);
public:
	void SetCustomEncryptor(UGenericNativeEncryptor * EncryptorIn);
	
	FString GetSigninURL(const ESocialSigninType& Type) const;

	void InitiateMobileSSO(const ESocialSigninType& Type);

	void UpdateMobileLogin(const FString& TokenizedUrl) const;
	
	void SocialLogin(const FString& IDTokenIn) const;

	void EmailLogin(const FString& EmailIn);

	void GuestLogin(const bool ForceCreateAccountIn) const;

	/**
	 * Used to create & login a new account with PlayFab, Then OpenSession with Sequence
	 * @param UsernameIn Username
	 * @param EmailIn Email
	 * @param PasswordIn Password
	 */
	void PlayFabRegisterAndLogin(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const;

	/**
	 * Used to login with PlayFab, Then OpenSession with Sequence
	 * @param UsernameIn Username
	 * @param PasswordIn Password
	 */
	void PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) const;

	/**
	 * Used to complete Email based authentication, whether it be for normal Authentication OR Federation
	 * @param CodeIn Received Code from email
	 */
	void EmailLoginCode(const FString& CodeIn) const;
	
	void FederateEmail(const FString& EmailIn);
	
	void FederateOIDCIdToken(const FString& IdTokenIn) const;

	void InitiateMobileFederateOIDC(const ESocialSigninType& Type);

	void FederatePlayFabNewAccount(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const;
	
	void FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) const;

	FStoredCredentials_BE GetStoredCredentials() const;

	void StoreCredentials(const FCredentials_BE& Credentials) const;

	void ClearStoredCredentials() const;
	
private:
	
	FString BuildRedirectPrefix() const;
	
	bool GetStoredCredentials(FCredentials_BE * Credentials) const;

	FString GenerateSigninURL(const ESocialSigninType& Type) const;

	FString GenerateRedirectURL(const ESocialSigninType& Type) const;

	void InitializeSequence(const FCredentials_BE& Credentials) const;

	//PlayFab RPC//

	/**
	 * Used to Login a user with PlayFab
	 * @param UsernameIn Username to login with
	 * @param PasswordIn password for account
	 * @param OnSuccess Called when successful (returns sessionTicket)
	 * @param OnFailure Called when unsuccessful (returns error)
	 */
	static void PlayFabLoginRPC(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Used to Create a new user & log them in with PlayFab
	 * @param UsernameIn New Username
	 * @param EmailIn New Email
	 * @param PasswordIn New Password
	 * @param OnSuccess Called when successful (returns sessionTicket)
	 * @param OnFailure Called when unsuccessful (returns error)
	 */
	static void PlayFabNewAccountLoginRPC(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	static FString GeneratePlayFabUrl();
	
	static FString GeneratePlayFabRegisterUrl();
	
	static void PlayFabRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	
	//PlayFab RPC//
};