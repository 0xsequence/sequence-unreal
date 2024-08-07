// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Authenticator.h"
#include "Misc/Guid.h"
#include "Misc/Base64.h"
#include "Types/Wallet.h"
#include "StorableCredentials.h"
#include "Kismet/GameplayStatics.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceEncryptor.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "PlayFabSendIntent.h"
#include "RequestHandler.h"
#include "WebBrowserModule.h"
#include "SequenceRPCManager.h"
#include "Native/NativeOAuth.h"
#include "NativeEncryptors/AppleEncryptor.h"
#include "NativeEncryptors/AndroidEncryptor.h"
#include "NativeEncryptors/WindowsEncryptor.h"
#include "PlayFabResponseIntent.h"
#include "Sequence/SequenceAPI.h"

UAuthenticator::UAuthenticator()
{
	this->SessionWallet = UWallet::Make();//Generate a new Random UWallet
	this->StateToken = FGuid::NewGuid().ToString();
	this->SequenceRPCManager = USequenceRPCManager::Make(this->SessionWallet);
	
	if constexpr (PLATFORM_ANDROID)
	{
		this->Encryptor = NewObject<UAndroidEncryptor>();
	}
	else if constexpr (PLATFORM_MAC)
	{
		this->Encryptor = NewObject<UAppleEncryptor>();
	}
	else if constexpr (PLATFORM_WINDOWS)
	{
		this->Encryptor = NewObject<UWindowsEncryptor>();
	}
	else if constexpr (PLATFORM_IOS)
	{
		this->Encryptor = NewObject<UAppleEncryptor>();
	}
}

void UAuthenticator::SetCustomEncryptor(UGenericNativeEncryptor * EncryptorIn)
{
	this->Encryptor = EncryptorIn;
	if (this->Encryptor)
	{
		const FString EncryptorName = this->Encryptor->GetClass()->GetName();
		UE_LOG(LogTemp,Display,TEXT("Setting custom encryptor to: %s"),*EncryptorName);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("Received null instead of a pointer to an Encryptor Object using fallback encryptor"));	
	}
}

void UAuthenticator::ClearStoredCredentials() const
{
	const FCredentials_BE BlankCredentials;
	this->StoreCredentials(BlankCredentials);
}

void UAuthenticator::StoreCredentials(const FCredentials_BE& Credentials) const
{
	if (UStorableCredentials* StorableCredentials = Cast<UStorableCredentials>(UGameplayStatics::CreateSaveGameObject(UStorableCredentials::StaticClass())))
	{
		const FString CTS_Json = UIndexerSupport::StructToString<FCredentials_BE>(Credentials);
		const int32 CTS_Json_Length = CTS_Json.Len();

		if (Encryptor)
		{//Use set encryptor
			StorableCredentials->EK = this->Encryptor->Encrypt(CTS_Json);
			StorableCredentials->KL = CTS_Json_Length;
		}
		else
		{//Use the fallback
			StorableCredentials->EK = USequenceEncryptor::Encrypt(CTS_Json);
			StorableCredentials->KL = CTS_Json_Length;
		}

		if (UGameplayStatics::SaveGameToSlot(StorableCredentials, this->SaveSlot, this->UserIndex))
		{
			UE_LOG(LogTemp, Display, TEXT("Data Saved Correctly!"));
		}
	}
}

FString UAuthenticator::BuildRedirectPrefix() const
{
	const FString Redirect = UConfigFetcher::GetConfigVar(UConfigFetcher::RedirectUrl);
	if (Redirect.EndsWith(TEXT("/"),ESearchCase::IgnoreCase))
		return Redirect + this->RedirectPrefixTrailer;
	else
		return Redirect + "/" + this->RedirectPrefixTrailer;
}

bool UAuthenticator::GetStoredCredentials(FCredentials_BE* Credentials) const
{
	bool ret = false;
	if (const UStorableCredentials* LoadedCredentials = Cast<UStorableCredentials>(UGameplayStatics::LoadGameFromSlot(this->SaveSlot, this->UserIndex)))
	{
		FString CTR_Json = "";
		if (Encryptor)
		{//Use set encryptor
			CTR_Json = Encryptor->Decrypt(LoadedCredentials->EK);
		}
		else
		{//Use the fallback
			CTR_Json = USequenceEncryptor::Decrypt(LoadedCredentials->EK, LoadedCredentials->KL);
		}

		ret = UIndexerSupport::JSONStringToStruct<FCredentials_BE>(CTR_Json, Credentials);
		ret &= Credentials->RegisteredValid();
	}
	return ret;
}

void UAuthenticator::CallAuthRequiresCode() const
{
	if (this->AuthRequiresCode.IsBound())
		this->AuthRequiresCode.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Failure: nothing bound to delegate: AuthRequiresCode]"));
}

void UAuthenticator::CallAuthFailure() const
{
	if (this->AuthFailure.IsBound())
		this->AuthFailure.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: AuthFailure]"));
}

void UAuthenticator::CallAuthSuccess() const
{
	if (this->AuthSuccess.IsBound())
		this->AuthSuccess.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: AuthSuccess]"));
}

void UAuthenticator::UpdateMobileLogin(const FString& TokenizedUrl) const
{
	//we need to parse out the id_token out of TokenizedUrl
	TArray<FString> UrlParts;
	TokenizedUrl.ParseIntoArray(UrlParts,TEXT("?"),true);
	for (FString part: UrlParts)
	{
		if (part.Contains("id_token",ESearchCase::IgnoreCase))
		{
			TArray<FString> ParameterParts;
			part.ParseIntoArray(ParameterParts,TEXT("&"),true);
			for (FString parameter : ParameterParts)
			{
				if (parameter.Contains("id_token",ESearchCase::IgnoreCase))
				{
					const FString Token = parameter.RightChop(9);//we chop off: id_token
					SocialLogin(Token);
					return;
				}//find id_token
			}//parse out &
		}//find id_token
	}//parse out ?
}

void UAuthenticator::InitiateMobileSSO(const ESocialSigninType& Type)
{
#if PLATFORM_ANDROID
	switch (Type)
	{
	case ESocialSigninType::Apple:
		NativeOAuth::RequestAuthWebView(GenerateSigninURL(Type),GenerateRedirectURL(Type), this);
		break;
	case ESocialSigninType::Google:
		NativeOAuth::SignInWithGoogle(UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID), this);
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
#endif
	
#if PLATFORM_IOS
	FString clientID = UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type) + "&client_id=" + UConfigFetcher::GetConfigVar(UConfigFetcher::AppleClientID);
	switch (Type)
	{
	case ESocialSigninType::Apple:
		NativeOAuth::SignInWithApple(clientID this);
		break;
	case ESocialSigninType::Google:
		NativeOAuth::SignInWithGoogle_IOS(this->GetSigninURL(Type),UrlScheme,this);
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
#endif
}

FString UAuthenticator::GetSigninURL(const ESocialSigninType& Type) const
{
	FString SigninURL = "";
	
	if (this->SSOProviderMap.Contains(Type))
	{
		SigninURL = this->GenerateSigninURL(Type);
	}
	else
	{
		const FString SSOType = UEnum::GetValueAsString(Type);
		UE_LOG(LogTemp, Error, TEXT("No Entry for SSO type: [%s] in SSOProviderMap"),*SSOType);
	}

	//clear web cache here so signin will be clean each time!
	if (this->PurgeCache)
	{
		if (const IWebBrowserSingleton* WebBrowserSingleton = IWebBrowserModule::Get().GetSingleton())
		{
			const TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager();
			if (CookieManager.IsValid())
			{
				CookieManager->DeleteCookies();
			}
		}
	}
	return SigninURL;
}

void UAuthenticator::SocialLogin(const FString& IDTokenIn) const
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->InitializeSequence(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("OIDC Auth Error: %s"), *Error.Message);
		this->CallAuthFailure();
	};

	this->SequenceRPCManager->OpenOIDCSession(IDTokenIn, false, OnSuccess, OnFailure);
}

void UAuthenticator::EmailLogin(const FString& EmailIn) const
{
	const TFunction<void()> OnSuccess = [this]
	{
		this->CallAuthRequiresCode();
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->CallAuthFailure();
	};

	this->SequenceRPCManager->InitEmailAuth(EmailIn.ToLower(),OnSuccess,OnFailure);
}

void UAuthenticator::GuestLogin(const bool ForceCreateAccountIn) const
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->InitializeSequence(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Guest Auth Error: %s"), *Error.Message);
		this->CallAuthFailure();
	};

	this->SequenceRPCManager->OpenGuestSession(ForceCreateAccountIn,OnSuccess,OnFailure);
}

void UAuthenticator::PlayFabRegisterAndLogin(const FString& UsernameIn, const FString& EmailIn, const FString& PasswordIn) const
{
	const TFunction<void(FString)> OnResponse = [this](const FString& Response)
	{
		const FPlayFabRegisterUserResponse ParsedResponse = UIndexerSupport::JSONStringToStruct<FPlayFabRegisterUserResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
			{
				this->InitializeSequence(Credentials);
			};

			const FFailureCallback OnFailure = [this](const FSequenceError& Error)
			{
				UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *Error.Message);
				this->CallAuthFailure();
			};
			
			this->SequenceRPCManager->OpenPlayFabSession(ParsedResponse.Data.SessionTicket,false, OnSuccess, OnFailure);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Response);
			this->CallAuthFailure();
		}
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Error.Message);
	};

	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	const FPlayFabRegisterUser RegisterUser(TitleId, EmailIn, PasswordIn, UsernameIn);
	const FString RequestBody = UIndexerSupport::StructToPartialSimpleString(RegisterUser);
	
	PlayFabRPC(GeneratePlayFabRegisterUrl(), RequestBody, OnResponse, OnFailure);
}

void UAuthenticator::PlayFabLogin(const FString& UsernameIn, const FString& PasswordIn) const
{
	const TFunction<void(FString)> OnResponse = [this](const FString& Response)
	{
		const FPlayFabLoginUserResponse ParsedResponse = UIndexerSupport::JSONStringToStruct<FPlayFabLoginUserResponse>(Response);

		if (ParsedResponse.IsValid())
		{
			const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
			{
				this->InitializeSequence(Credentials);
			};

			const FFailureCallback OnFailure = [this](const FSequenceError& Error)
			{
				UE_LOG(LogTemp, Warning, TEXT("Error: %s"), *Error.Message);
				this->CallAuthFailure();
			};
			
			this->SequenceRPCManager->OpenPlayFabSession(ParsedResponse.Data.SessionTicket,false, OnSuccess, OnFailure);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Response);
			this->CallAuthFailure();
		}
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error Response: %s"), *Error.Message);
	};

	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	const FPlayFabLoginUser LoginUser(PasswordIn,TitleId,UsernameIn);
	const FString RequestBody = UIndexerSupport::StructToPartialSimpleString(LoginUser);
	
	PlayFabRPC(GeneratePlayFabUrl(), RequestBody, OnResponse, OnFailure);
}

FString UAuthenticator::GenerateRedirectURL(const ESocialSigninType& Type) const
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

FString UAuthenticator::GenerateSigninURL(const ESocialSigninType& Type) const
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

void UAuthenticator::InitializeSequence(const FCredentials_BE& Credentials) const
{
	if (const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials); WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		this->StoreCredentials(Credentials);
		this->CallAuthSuccess();
	}
	else
	{
		this->CallAuthFailure();
	}
}

FString UAuthenticator::GeneratePlayFabUrl()
{
	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	return "https://" + TitleId + ".playfabapi.com/Client/LoginWithPlayFab";
}

FString UAuthenticator::GeneratePlayFabRegisterUrl()
{
	const FString TitleId = UConfigFetcher::GetConfigVar(UConfigFetcher::PlayFabTitleID);
	return "https://" + TitleId + ".playfabapi.com/Client/RegisterPlayFabUser";
}

void UAuthenticator::PlayFabRPC(const FString& Url, const FString& Content, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure)
{
	NewObject<URequestHandler>()
	->PrepareRequest()
	->WithUrl(Url)
	->WithHeader("Content-type", "application/json")
	->WithVerb("POST")
	->WithContentAsString(Content)
	->ProcessAndThen(OnSuccess, OnFailure);
}

void UAuthenticator::EmailLoginCode(const FString& CodeIn) const
{
	const TSuccessCallback<FCredentials_BE> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		this->InitializeSequence(Credentials);
	};

	const FFailureCallback OnFailure = [this](const FSequenceError& Error)
	{
		UE_LOG(LogTemp, Error, TEXT("Email Auth Error: %s"), *Error.Message);
		this->CallAuthFailure();
	};
	
	this->SequenceRPCManager->OpenEmailSession(CodeIn, false, OnSuccess, OnFailure);
}

FStoredCredentials_BE UAuthenticator::GetStoredCredentials() const
{	
	FCredentials_BE CredData;
	FCredentials_BE* Credentials = &CredData;
	const bool IsValid = this->GetStoredCredentials(Credentials);
	return FStoredCredentials_BE(IsValid, *Credentials);
}