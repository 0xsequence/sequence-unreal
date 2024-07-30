// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Authenticator.h"
#include "HttpModule.h"
#include "Misc/Guid.h"
#include "Misc/Base64.h"
#include "Eth/EthTransaction.h"
#include "Types/Wallet.h"
#include "StorableCredentials.h"
#include "Kismet/GameplayStatics.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceEncryptor.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "WebBrowserModule.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Interfaces/IHttpResponse.h"
#include "Native/NativeOAuth.h"
#include "NativeEncryptors/AppleEncryptor.h"
#include "NativeEncryptors/AndroidEncryptor.h"
#include "NativeEncryptors/WindowsEncryptor.h"
#include "Sequence/SequenceAPI.h"

UAuthenticator::UAuthenticator()
{
	this->SessionWallet = UWallet::Make();//Generate a new Random UWallet
	this->SessionId = this->SessionWallet->GetSessionId();
	this->SessionHash = this->SessionWallet->GetSessionHash();
	
	this->Nonce = this->SessionHash;
	this->StateToken = FGuid::NewGuid().ToString();
	FString ParsedJWT;
	FBase64::Decode(UConfigFetcher::GetConfigVar(UConfigFetcher::WaaSConfigKey),ParsedJWT);
	this->WaasSettings = UIndexerSupport::JSONStringToStruct<FWaasJWT>(ParsedJWT);

	//Right here we'd pass in this->WaasSettings to USequenceWallet to enable it for bootStrapping.
	//Alternatively is we could read it from disk AGAIN inside USequenceWallet. (this would probably be cleaner actually)
	
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

bool UAuthenticator::CanHandleEmailLogin() const
{
	bool ret = true;
	ret &= this->WaasSettings.GetEmailRegion().Len() > 0;
	ret &= this->WaasSettings.GetEmailClientId().Len() > 0;
	return ret;
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

void UAuthenticator::UpdateMobileLogin(const FString& TokenizedUrl)
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
		NativeOAuth::SignInWithGoogle(UConfigFetcher::GetConfigVar(UConfigFetcher::GoogleClientID),this->Nonce,this);
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
		NativeOAuth::SignInWithApple(clientID, this->Nonce, this);
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

	//clear webcache here so signin will be clean eachtime!
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

void UAuthenticator::SocialLogin(const FString& IDTokenIn)
{	
	this->Cached_IDToken = IDTokenIn;
	const FString SessionPrivateKey = BytesToHex(this->SessionWallet->GetWalletPrivateKey().Ptr(), this->SessionWallet->GetWalletPrivateKey().GetLength()).ToLower();
	const FCredentials_BE Credentials(this->WaasSettings.GetRPCServer(), this->WaasSettings.GetProjectId(), UConfigFetcher::GetConfigVar(UConfigFetcher::ProjectAccessKey),SessionPrivateKey,this->SessionId,this->Cached_IDToken,this->Cached_Email,WaasVersion);
	this->StoreCredentials(Credentials);
	this->AutoRegister(Credentials);
}

void UAuthenticator::EmailLogin(const FString& EmailIn)
{
	if (this->CanHandleEmailLogin())
	{
		this->ResetRetryEmailLogin();
		this->Cached_Email = EmailIn.ToLower();
		//Email Init
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Email based Auth not setup properly please ensure your WaaSConfigKey is configured properly"));
		this->CallAuthFailure();
	}
}

void UAuthenticator::GuestLogin()
{
}

FString UAuthenticator::GenerateRedirectURL(const ESocialSigninType& Type) const
{
	FString RedirectUrl = this->BuildRedirectPrefix() + "&nonce=" + this->Nonce + "&scope=openid+email&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
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
	FString SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ this->BuildRedirectPrefix() + "&nonce=" + this->Nonce + "&scope=openid+email&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
	switch (Type)
	{
	case ESocialSigninType::Google:
		break;
	case ESocialSigninType::Apple://For apple we have no scope, as well as the trailing response_mode
		SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ this->BuildRedirectPrefix() + "&nonce=" + this->Nonce + "&state=" + UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
		SigninUrl += "&response_mode=fragment";
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
	return SigninUrl;
}

FString UAuthenticator::GetISSClaim(const FString& JWT) const
{
	FString ISSClaim = "";

	TArray<FString> B64Json;
	JWT.ParseIntoArray(B64Json, TEXT("."), true);
	
	for (int i = 0; i < B64Json.Num(); i++)
	{
		FString T;
		FBase64::Decode(B64Json[i], T);
		B64Json[i] = T;
	}

	if (B64Json.Num() > 1)
	{
		const TSharedPtr<FJsonObject> json = this->ResponseToJson(B64Json[1]);
		FString iss;
		if (json.Get()->TryGetStringField(TEXT("iss"), iss))
		{
			if (iss.Contains("https://",ESearchCase::IgnoreCase))
				ISSClaim = iss.RightChop(8);
			else
				ISSClaim = iss;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[JWT didn't contain an ISS field]"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[JWT Parse Failure]"));
	}
	return ISSClaim;
}

FString UAuthenticator::ParseResponse(const FHttpResponsePtr& Response,bool WasSuccessful)
{
	FString Ret = "";

	if(WasSuccessful)
	{
		Ret = Response.Get()->GetContentAsString();
	}
	else
	{
		if(Response.IsValid())
			Ret = "Request is invalid!";
		else
			Ret = "Request failed: " + Response->GetContentAsString();
	}
	
	return Ret;
}

FString UAuthenticator::BuildYYYYMMDD(const FDateTime& Date)
{
	return Date.ToString(TEXT("%Y%m%d"));//YYYYMMDD
}

FString UAuthenticator::BuildFullDateTime(const FDateTime& Date)
{
	return Date.ToString(TEXT("%Y%m%dT%H%M%SZ"));//YYYYMMDDTHHMMSSZ
}

bool UAuthenticator::CanRetryEmailLogin()
{
	const bool CanRetry = this->EmailAuthCurrRetries > 0;
	this->EmailAuthCurrRetries--;
	return CanRetry;
}

void UAuthenticator::ResetRetryEmailLogin()
{
	this->EmailAuthCurrRetries = this->EmailAuthMaxRetries;
}

FString UAuthenticator::GenerateSignUpPassword()
{
	FString pw = "aB1%";
	constexpr int32 len = 12;
	for (int i = 0; i < len; i++)
	{
		pw += this->PWCharList[FMath::RandRange(0,this->PWCharList.Num()-1)];
	}
	return pw;
}

void UAuthenticator::AutoRegister(const FCredentials_BE& Credentials) const
{	
	const TFunction<void (FCredentials_BE)> OnSuccess = [this](const FCredentials_BE& Credentials)
	{
		if (Credentials.IsRegistered())
		{
			UE_LOG(LogTemp,Display,TEXT("Successfully Auto Registered Credentials"));
			this->CallAuthSuccess();
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("Failure During Auto Register, Credentials weren't registered"));
			this->CallAuthFailure();
		}
	};
	
	const TFunction<void (FSequenceError)> OnFailure = [this](const FSequenceError& Err)
	{
		UE_LOG(LogTemp,Display,TEXT("Failure During Auto Register: %s"),*Err.Message);
		this->CallAuthFailure();
	};

	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get(Credentials);
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		USequenceWallet * Wallet = WalletOptional.GetValue();
		Wallet->RegisterSession(OnSuccess,OnFailure);
	}
	else
	{
		this->CallAuthFailure();
	}
}

TSharedPtr<FJsonObject> UAuthenticator::ResponseToJson(const FString& Response)
{
	TSharedPtr<FJsonObject> responseObj;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Response), responseObj))
		return responseObj;
	else
		return nullptr;
}

void UAuthenticator::EmailLoginCode(const FString& CodeIn)
{
	
}

FStoredCredentials_BE UAuthenticator::GetStoredCredentials() const
{	
	FCredentials_BE CredData;
	FCredentials_BE* Credentials = &CredData;
	const bool IsValid = this->GetStoredCredentials(Credentials);
	return FStoredCredentials_BE(IsValid, *Credentials);
}