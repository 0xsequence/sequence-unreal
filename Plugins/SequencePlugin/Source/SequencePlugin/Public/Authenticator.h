// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Util/Structs/BE_Enums.h"
#include "Types/Wallet.h"
#include "Interfaces/IHttpRequest.h"
#include "Dom/JsonObject.h"
#include "ConfigFetcher.h"
#include "NativeEncryptors/GenericNativeEncryptor.h"
#include "Credentials.h"
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
	
private://Broadcast handlers
	void CallAuthRequiresCode() const;
	void CallAuthFailure() const;
	void CallAuthSuccess() const;
//vars
private:
	UPROPERTY()
	UGenericNativeEncryptor * Encryptor = nullptr;
	
	const FString SaveSlot = "Cr";
	const uint32 UserIndex = 0;

	UPROPERTY()
	FString SessionId = "";
	UPROPERTY()
	FString SessionHash = "";
	UPROPERTY()
	FString StateToken = "";
	UPROPERTY()
	FString Nonce = "";
	UPROPERTY()
	FString Cached_IDToken;
	UPROPERTY()
	FString Cached_Email;
	UPROPERTY()
	FString Cached_Issuer;
	
	TMap<ESocialSigninType, FSSOCredentials> SSOProviderMap ={
		{ESocialSigninType::Google,FSSOCredentials(GoogleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID))},
		{ESocialSigninType::Apple,FSSOCredentials(AppleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::AppleClientID))},
		{ESocialSigninType::Discord,FSSOCredentials(DiscordAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::DiscordClientID))},
		{ESocialSigninType::FaceBook,FSSOCredentials(FacebookAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::FacebookClientID))}};
	
	UPROPERTY()
	TArray<FString> PWCharList = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9"};
	const int32 EmailAuthMaxRetries = 2;
	UPROPERTY()
	int32 EmailAuthCurrRetries = EmailAuthMaxRetries;
	UPROPERTY()
	UWallet* SessionWallet;
	UPROPERTY()
	USequenceRPCManager * SequenceRPCManager = nullptr;
	UPROPERTY()
	bool PurgeCache = true;

	//Static Config variables
	inline static FString WaasVersion = "1.0.0";
	inline static FString UrlScheme = "powered-by-sequence";
	inline static FString GoogleAuthURL = "https://accounts.google.com/o/oauth2/auth";
	inline static FString FacebookAuthURL = "https://www.facebook.com/v18.0/dialog/oauth";
	inline static FString DiscordAuthURL = "https://discord.com/api/oauth2/authorize";
	inline static FString AppleAuthURL = "https://appleid.apple.com/auth/authorize";
	inline static FString RedirectPrefixTrailer = "oauth/callback";
private:
	UAuthenticator();
public:
	void SetCustomEncryptor(UGenericNativeEncryptor * EncryptorIn);
	
	FString GetSigninURL(const ESocialSigninType& Type) const;

	void InitiateMobileSSO(const ESocialSigninType& Type);

	void UpdateMobileLogin(const FString& TokenizedUrl);
	
	void SocialLogin(const FString& IDTokenIn);

	void EmailLogin(const FString& EmailIn);

	void GuestLogin();

	void EmailLoginCode(const FString& CodeIn);

	FStoredCredentials_BE GetStoredCredentials() const;

	void StoreCredentials(const FCredentials_BE& Credentials) const;

	void ClearStoredCredentials() const;
private:
	FString BuildRedirectPrefix() const;
	
	bool CanHandleEmailLogin() const;
	
	bool GetStoredCredentials(FCredentials_BE * Credentials) const;
	
	FString GetISSClaim(const FString& JWT) const;

	bool CanRetryEmailLogin();

	void ResetRetryEmailLogin();

	FString GenerateSigninURL(const ESocialSigninType& Type) const;

	FString GenerateRedirectURL(const ESocialSigninType& Type) const;

	FString GenerateSignUpPassword();

	FString BuildYYYYMMDD(const FDateTime& Date);

	static FString BuildFullDateTime(const FDateTime& Date);

	//RPC Calls//
	static FString ParseResponse(const FHttpResponsePtr& Response,bool WasSuccessful);

	void AutoRegister(const FCredentials_BE& Credentials) const;

	//RPC Calls//
	static TSharedPtr<FJsonObject> ResponseToJson(const FString& Response);
};