// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Util/Structs/BE_Enums.h"
#include "Dom/JsonObject.h"
#include "ConfigFetcher.h"
#include "NativeEncryptors/GenericNativeEncryptor.h"
#include "Credentials.h"
#include "Sequence/SequenceFederationSupport.h"
#include "Util/Async.h"
#include "Authenticator.generated.h"

class USequenceRPCManager;

enum ESeqAuthType
{
	Normal,//Normal Usage
	Federation,//Normal Federation Usage
	FederationInUse,//Federating an Account that's already logged in Usage
	Force,//Force Create Account Usage
};

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFederateOrForce, const FFederationSupportData&, FederationData);

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
	UPROPERTY()
	FOnFederateOrForce FederateOrForce;
	
private://Broadcast handlers
	void CallAuthRequiresCode() const;
	void CallAuthFailure() const;
	void CallAuthSuccess() const;
	void CallFederateSuccess() const;
	void CallFederateFailure(const FString& ErrorMessageIn) const;
	void CallFederateOrForce(const FFederationSupportData& FederationData) const;
//vars
private:
	UPROPERTY()
	UGenericNativeEncryptor * Encryptor = nullptr;
	
	const FString SaveSlot = "Cr";
	const uint32 UserIndex = 0;
	FString StateToken = "";

	/**
	 * Used to change behaviour
	 * if the user is federating accounts (True)
	 * OR
	 * if the user is logging in normally (False)
	 */
	bool IsFederating = false;

	/**
	 * Used to change behaviour
	 * If the user is federating an account in Use (True)
	 * OR
	 * if the user is logging in normally (False)
	 */
	bool IsFederatingSessionInUse = false;

	/**
	 * Used to change behaviour for account forcing
	 * 2 Cases where we have to use this:
	 * 1) Initiate mobile OIDC Auth
	 * 2) Email Auth
	 * These 2 calls both are done in an Async manner where cannot directly hand state from the initial call
	 * to the finalizing call.
	 */
	bool IsForcing = false;
	
	TMap<ESocialSigninType, FSSOCredentials> SSOProviderMap ={
		{ESocialSigninType::Google,FSSOCredentials(GoogleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID))},
		{ESocialSigninType::Apple,FSSOCredentials(AppleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::AppleClientID))},
		{ESocialSigninType::Discord,FSSOCredentials(DiscordAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::DiscordClientID))},
		{ESocialSigninType::FaceBook,FSSOCredentials(FacebookAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::FacebookClientID))}};
	
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
	
	void InitiateMobileSSO_Internal(const ESocialSigninType& Type);

	/**
	 * Reads from the IsForcing State and resets it returning the
	 * UAuthenticator back to its default state
	 * @return the state read from IsForcing prior to resetting it
	 */
	bool ReadAndResetIsForcing();
public:
	
	/**
	 * Sets a custom encryptor
	 * @param EncryptorIn Encryptor to use
	 */
	void SetCustomEncryptor(UGenericNativeEncryptor * EncryptorIn);

	/**
	 * Used to get an OIDC Login Url
	 * @param Type Type of OIDC Url need
	 * @return An OIDC login Url of the specified Type
	 */
	FString GetSigninURL(const ESocialSigninType& Type) const;

	/**
	 * Used to initiate mobile Login
	 * @param Type Type of OIDC to conduct on Mobile
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void InitiateMobileSSO(const ESocialSigninType& Type, const bool ForceCreateAccountIn);

	/**
	 * Internal Mobile Login call. Used to complete mobile login once a tokenized URL is received
	 * @param TokenizedUrl The URL containing an IdToken
	 */
	void UpdateMobileLogin(const FString& TokenizedUrl);

	/**
	 * Internal Mobile Login Call. Used to complete mobile Login once an IdToken has been received
	 * @param IdTokenIn IdToken Received from mobile login
	 */
	void UpdateMobileLogin_IdToken(const FString& IdTokenIn);

	/**
	 * Used to initiate OIDC login
	 * @param IDTokenIn OIDC Token granted from login
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void SocialLogin(const FString& IDTokenIn, const bool ForceCreateAccountIn);

	/**
	 * Used to initiate email login
	 * @param EmailIn Email
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void EmailLogin(const FString& EmailIn, const bool ForceCreateAccountIn);

	/**
	 * Used to login as a Guest into Sequence
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void GuestLogin(const bool ForceCreateAccountIn) const;

	/**
	 * Used to create & login a new account with PlayFab, Then OpenSession with Sequence
	 * @param UsernameIn Username
	 * @param EmailIn Email
	 * @param PasswordIn Password
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void PlayFabRegisterAndLogin(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const bool ForceCreateAccountIn);

	/**
	 * Used to login with PlayFab, Then OpenSession with Sequence
	 * @param UsernameIn Username
	 * @param PasswordIn Password
	 * @param ForceCreateAccountIn Force create account if it already exists
	 */
	void PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, const bool ForceCreateAccountIn);

	/**
	 * Used to complete Email based authentication, whether it be for normal Authentication OR Federation
	 * @param CodeIn Received Code from email
	 */
	void EmailLoginCode(const FString& CodeIn);

	/**
	 * Used To Federate an Email (WIP)
	 * @param EmailIn Email to federate
	 */
	void FederateEmail(const FString& EmailIn);

	/**
	 * Used to Federate an OIDC Login
	 * @param IdTokenIn OIDC Token To federate
	 */
	void FederateOIDCIdToken(const FString& IdTokenIn) const;

	/**
	 * Used to initiate OIDC account federation on mobile
	 * @param Type Type of OIDC account to federate
	 */
	void InitiateMobileFederateOIDC(const ESocialSigninType& Type);

	/**
	 * Used to federate a new PlayFab account
	 * @param UsernameIn PlayFab Username
	 * @param EmailIn PlayFab Email
	 * @param PasswordIn PlayFab Password
	 */
	void FederatePlayFabNewAccount(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const;

	/**
	 * Used to federate an existing account on PlayFab
	 * @param UsernameIn PlayFab Username
	 * @param PasswordIn PlayFab Password
	 */
	void FederatePlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) const;

	/**
	 * Used to get stored credentials from Disk
	 * @return Stored Credentials
	 */
	FStoredCredentials_BE GetStoredCredentials() const;

	/**
	 * Used to store Credentials on Disk
	 * @param Credentials Credentials to be Stored
	 */
	void StoreCredentials(const FCredentials_BE& Credentials) const;

	/**
	 * Clears stored credentials on disk with blanks
	 */
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