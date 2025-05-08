

#include "Sequence/SequenceSessions.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "PlayFabResponseIntent.h"
#include "PlayFabSendIntent.h"
#include "RequestHandler.h"
#include "SequenceRPCManager.h"
#include "WebBrowserModule.h"
#include "Native/NativeOAuth.h"
#include "Sequence/SequenceSdk.h"
#include "Util/Log.h"

USequenceSessions::USequenceSessions()
{
	this->RPCManager = USequenceRPCManager::Make(false);
	this->CredentialsStorage = NewObject<UCredentialsStorage>();
	this->StateToken = FGuid::NewGuid().ToString();
	this->ForceCreateAccount = true;

#if PLATFORM_ANDROID || PLATFORM_IOS
	this->IsBrowserRequired = false;
#else
	this->IsBrowserRequired = true;
#endif
}

FString USequenceSessions::GetGoogleSignInUrl() const
{
	return this->GetSigninURL(ESocialSigninType::Google);
}

FString USequenceSessions::GetAppleSignInUrl() const
{
	return this->GetSigninURL(ESocialSigninType::Apple);
}

void USequenceSessions::GetGoogleTokenId(INativeAuthCallback* CallbackHandler)
{
#if PLATFORM_ANDROID
	NativeOAuth::SignInWithGoogle(UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID), CallbackHandler);
#elif PLATFORM_IOS
	NativeOAuth::SignInWithGoogle_IOS(this->GetSigninURL(ESocialSigninType::Google),UrlScheme, CallbackHandler);
#else
	SEQ_LOG(Error, TEXT("GetGoogleTokenId is unavailable on this platform."));
#endif
}

void USequenceSessions::GetAppleTokenId(INativeAuthCallback* CallbackHandler)
{
#if PLATFORM_ANDROID
	const FString RequestUrl = GenerateSigninURL(ESocialSigninType::Apple);
	const FString RedirectUrl = GenerateRedirectURL(ESocialSigninType::Apple);
	NativeOAuth::RequestAuthWebView(RequestUrl, RedirectUrl, CallbackHandler);
#elif PLATFORM_IOS
	const FString ClientId = UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(ESocialSigninType::Apple) + "&client_id=" + UConfigFetcher::GetConfigVar(UConfigFetcher::AppleClientID);
	NativeOAuth::SignInWithApple(ClientId, CallbackHandler);
#else
	SEQ_LOG(Error, TEXT("GetGoogleTokenId is unavailable on this platform."));
#endif
}

bool USequenceSessions::CheckExistingSession() const
{
	const FStoredCredentials_BE Credentials = this->CredentialsStorage->GetStoredCredentials();
	return Credentials.GetValid();
}

void USequenceSessions::StartGuestSession(const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const TSuccessCallback<FCredentials_BE> OnApiSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
	{
		this->CredentialsStorage->StoreCredentials(Credentials);
		OnSuccess();
	};

	this->RPCManager->OpenGuestSession(this->ForceCreateAccount, OnApiSuccess, OnFailure);
}

void USequenceSessions::StartEmailLogin(const FString& Email, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure) const
{
	if (this->CheckExistingSession())
	{
		this->RPCManager->InitEmailFederation(Email.ToLower(), OnSuccess, OnFailure);
	}
	else
	{
		this->RPCManager->InitEmailAuth(Email.ToLower(), OnSuccess, OnFailure);
	}
}

void USequenceSessions::PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired)
{
	const TSuccessCallback<FString> OnApiSuccess = [this, OnSuccess, OnFailure, OnFederationRequired](const FString& SessionTicket)
	{
		this->PlayFabAuthenticateWithSessionTicket(SessionTicket, OnSuccess, OnFailure, OnFederationRequired);
	};

	this->PlayFabLoginRpc(UsernameIn, PasswordIn, OnApiSuccess, OnFailure);
}

void USequenceSessions::SetForceCreateAccount(const bool NewForceCreateAccount)
{
	this->ForceCreateAccount = NewForceCreateAccount;
}

void USequenceSessions::StartOidcSession(const FString& IdToken, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired) const
{
	const TSuccessCallback<FCredentials_BE> OnApiSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
	{
		this->CredentialsStorage->StoreCredentials(Credentials);
		OnSuccess();
	};

	if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
	{
		this->RPCManager->FederateOIDCSession(StoredCredentials.GetCredentials().GetWalletAddress(),IdToken, OnSuccess, OnFailure);
	}
	else
	{
		this->RPCManager->OpenOIDCSession(IdToken, this->ForceCreateAccount, OnApiSuccess, OnFailure, OnFederationRequired);
	}
}

void USequenceSessions::ConfirmEmailLoginWithCode(const FString& Code, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired) const
{
	const TSuccessCallback<FCredentials_BE> OnApiSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
	{
		this->CredentialsStorage->StoreCredentials(Credentials);
		OnSuccess();
	};

	if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
	{
		this->RPCManager->FederateEmailSession(StoredCredentials.GetCredentials().GetWalletAddress(), Code, OnSuccess, OnFailure);
	}
	else
	{
		this->RPCManager->OpenEmailSession(Code, this->ForceCreateAccount, OnApiSuccess, OnFailure, OnFederationRequired);	
	}
}

void USequenceSessions::PlayFabAuthenticateWithSessionTicket(const FString& SessionTicket, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired) const
{
	if (FStoredCredentials_BE StoredCredentials = this->CredentialsStorage->GetStoredCredentials(); StoredCredentials.GetValid())
	{
		this->RPCManager->FederatePlayFabSession(StoredCredentials.GetCredentials().GetWalletAddress(), SessionTicket, OnSuccess, OnFailure);
	}
	else
	{
		const TSuccessCallback<FCredentials_BE> OnOpenSuccess = [this, OnSuccess](const FCredentials_BE& Credentials)
		{
			this->CredentialsStorage->StoreCredentials(Credentials);
			OnSuccess();
		};
		
		this->RPCManager->OpenPlayFabSession(SessionTicket, this->ForceCreateAccount, OnOpenSuccess, OnFailure, OnFederationRequired);
	}
}

void USequenceSessions::PlayFabRegistration(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TFunction<void()>& OnSuccess, const FFailureCallback& OnFailure, const TFunction<void(FFederationSupportData)>& OnFederationRequired)
{
	const TSuccessCallback<FString> OnApiSuccess = [this, OnSuccess, OnFailure, OnFederationRequired](const FString& SessionTicket)
	{
		this->PlayFabAuthenticateWithSessionTicket(SessionTicket, OnSuccess, OnFailure, OnFederationRequired);
	};

	this->PlayFabNewAccountLoginRpc(UsernameIn, EmailIn, PasswordIn, OnApiSuccess, OnFailure);
}

FString USequenceSessions::GetSigninURL(const ESocialSigninType& Type) const
{
	FString SigninURL = "";
	
	if (this->SSOProviderMap.Contains(Type))
	{
		SigninURL = this->GenerateSigninURL(Type);
	}
	else
	{
		const FString SSOType = UEnum::GetValueAsString(Type);
		SEQ_LOG(Error, TEXT("No Entry for SSO type: [%s] in SSOProviderMap"),*SSOType);
	}

	if (const IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton())
	{
		const TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
		if (CookieManager.IsValid())
		{
			CookieManager->DeleteCookies();
		}
	}
	
	return SigninURL;
}

FString USequenceSessions::BuildRedirectPrefix() const
{
	const FString Redirect = SequenceSdk::GetRedirectUrl();
	if (Redirect.EndsWith(TEXT("/"),ESearchCase::IgnoreCase))
		return Redirect + this->RedirectPrefixTrailer;
	else
		return Redirect + "/" + this->RedirectPrefixTrailer;
}

FString USequenceSessions::GenerateSigninURL(const ESocialSigninType& Type) const
{
	const FString AuthClientId = SSOProviderMap[Type].ClientID;
	const FString AuthUrl = SSOProviderMap[Type].URL;
	FString SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ this->BuildRedirectPrefix() + "&scope=openid+email&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
	switch (Type)
	{
	case ESocialSigninType::Google:
		break;
	case ESocialSigninType::Apple://For apple we have no scope, as well as the trailing response_mode
		SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ this->BuildRedirectPrefix() + "&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
		SigninUrl += "&response_mode=fragment";
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
	return SigninUrl;
}

FString USequenceSessions::GenerateRedirectURL(const ESocialSigninType& Type) const
{
	FString RedirectUrl = this->BuildRedirectPrefix() + "&scope=openid+email&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
	switch (Type)
	{
	case ESocialSigninType::Google:
		break;
	case ESocialSigninType::Apple:
		RedirectUrl = this->BuildRedirectPrefix();
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
	return RedirectUrl;
}

void USequenceSessions::PlayFabNewAccountLoginRpc(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TFunction<void(FString)> OnSuccessResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		if (const FPlayFabRegisterUserResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FPlayFabRegisterUserResponse>(Response); ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Data.SessionTicket);
		}
		else
		{
			OnFailure(FSequenceError(ResponseParseError,Response));
		}
	};

	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	const FPlayFabRegisterUser RegisterUser(TitleId, EmailIn, PasswordIn, UsernameIn);
	const FString RequestBody = USequenceSupport::StructToPartialSimpleString(RegisterUser);
	const FString Url = "https://" + TitleId + ".playfabapi.com/Client/RegisterPlayFabUser";
	
	this->PlayFabRpc(Url, RequestBody, OnSuccessResponse, OnFailure);
}

void USequenceSessions::PlayFabLoginRpc(const FString& UsernameIn, const FString& PasswordIn, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	const TFunction<void(FString)> OnSuccessResponse = [OnSuccess, OnFailure](const FString& Response)
	{
		if (const FPlayFabLoginUserResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FPlayFabLoginUserResponse>(Response); ParsedResponse.IsValid())
		{
			OnSuccess(ParsedResponse.Data.SessionTicket);
		}
		else
		{
			OnFailure(FSequenceError(ResponseParseError,Response));
		}
	};

	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	const FPlayFabLoginUser LoginUser(PasswordIn,TitleId,UsernameIn);
	const FString RequestBody = USequenceSupport::StructToPartialSimpleString(LoginUser);
	const FString Url = "https://" + TitleId + ".playfabapi.com/Client/LoginWithPlayFab";
	
	this->PlayFabRpc(Url, RequestBody, OnSuccessResponse, OnFailure);
}

void USequenceSessions::PlayFabRpc(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithVerb("POST")
		->WithContentAsString(Content)
		->ProcessAndThen(*this->RPCManager->Validator, OnSuccess, OnFailure);
}
