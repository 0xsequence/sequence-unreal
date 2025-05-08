#pragma once

#include "CoreMinimal.h"
#include "ConfigFetcher.h"
#include "INativeAuthCallback.h"
#include "SequenceFederationSupport.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "Util/CredentialsStorage.h"
#include "Util/Structs/BE_Enums.h"
#include "SequenceSessions.generated.h"

class USequenceRPCManager;

USTRUCT()
struct SEQUENCEPLUGIN_API FssoCredentials
{
	GENERATED_USTRUCT_BODY()
	
	FString URL = "";
	FString ClientID = "";
	
	FssoCredentials(){}
	
	FssoCredentials(const FString& URLIn, const FString& ClientIDIn)
	{
		URL = URLIn;
		ClientID = ClientIDIn;
	}
};

UCLASS()
class SEQUENCEPLUGIN_API USequenceSessions : public UObject
{
	GENERATED_BODY()

public:
	USequenceSessions();

	FString GetGoogleSignInUrl() const;

	FString GetAppleSignInUrl() const;

	void GetGoogleTokenId(INativeAuthCallback* CallbackHandler);

	void GetAppleTokenId(INativeAuthCallback* CallbackHandler);
	
	bool CheckExistingSession() const;
	
	void StartEmailLogin(const FString& EmailIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;

	void ConfirmEmailLoginWithCode(const FString& CodeIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired) const;

	void StartOidcSession(const FString& IdToken, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired) const;

	void PlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired);

	void PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired);

	void PlayFabAuthenticateWithSessionTicket(const FString& SessionTicket, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired) const;

	void StartGuestSession(const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const;

	void SetForceCreateAccount(const bool NewForceCreateAccount);
	
	bool IsBrowserRequired;

private:
	inline static FString UrlScheme = "powered-by-sequence";
	inline static FString GoogleAuthURL = "https://accounts.google.com/o/oauth2/auth";
	inline static FString FacebookAuthURL = "https://www.facebook.com/v18.0/dialog/oauth";
	inline static FString DiscordAuthURL = "https://discord.com/api/oauth2/authorize";
	inline static FString AppleAuthURL = "https://appleid.apple.com/auth/authorize";
	inline static FString RedirectPrefixTrailer = "oauth/callback";
	
	TMap<ESocialSigninType, FssoCredentials> SSOProviderMap ={
		{ESocialSigninType::Google,FssoCredentials(GoogleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID))},
		{ESocialSigninType::Apple,FssoCredentials(AppleAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::AppleClientID))},
		{ESocialSigninType::Discord,FssoCredentials(DiscordAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::DiscordClientID))},
		{ESocialSigninType::FaceBook,FssoCredentials(FacebookAuthURL,UConfigFetcher::GetConfigVar(UConfigFetcher::FacebookClientID))}};
	
	FString GetSigninURL(const ESocialSigninType& Type) const;
	FString BuildRedirectPrefix() const;
	FString GenerateSigninURL(const ESocialSigninType& Type) const;
	FString GenerateRedirectURL(const ESocialSigninType& Type) const;

	void PlayFabNewAccountLoginRpc(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void PlayFabLoginRpc(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
	void PlayFabRpc(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);

	FString StateToken;
	bool ForceCreateAccount;

	UPROPERTY()
	USequenceRPCManager* RPCManager;

	UPROPERTY()
	UCredentialsStorage* CredentialsStorage;
};
