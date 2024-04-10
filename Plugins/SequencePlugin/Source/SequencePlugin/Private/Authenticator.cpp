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
#include "Sequence/SequenceAPI.h"

UAuthenticator::UAuthenticator()
{
	this->SessionWallet = UWallet::Make();//Generate a new Random UWallet
	this->SessionId = "0x00" + this->SessionWallet->GetWalletAddress().ToHex().ToLower();

	const FHash256 SessionHashBytes = FHash256::New();
	const FUnsizedData EncodedSigningData = StringToUTF8(this->SessionId);
	Keccak256::getHash(EncodedSigningData.Arr.Get()->GetData(), EncodedSigningData.GetLength(), SessionHashBytes.Ptr());
	this->SessionHash = "0x" + SessionHashBytes.ToHex().ToLower();
	
	this->Nonce = this->SessionHash;
	this->StateToken = FGuid::NewGuid().ToString();
	FString ParsedJWT;
	FBase64::Decode(FSequenceConfig::WaaSTenantKey,ParsedJWT);
	UE_LOG(LogTemp, Display, TEXT("Decoded Data: %s"),*ParsedJWT);

	this->WaasSettings = UIndexerSupport::JSONStringToStruct<FWaasJWT>(ParsedJWT);
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

		StorableCredentials->EK = USequenceEncryptor::Encrypt(CTS_Json);
		StorableCredentials->KL = CTS_Json_Length;

		if (UGameplayStatics::SaveGameToSlot(StorableCredentials, this->SaveSlot, this->UserIndex))
		{
			UE_LOG(LogTemp, Display, TEXT("Data Saved Correctly!"));
		}
	}
}

bool UAuthenticator::CanHandleEmailLogin()
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
		const FString CTR_Json = USequenceEncryptor::Decrypt(LoadedCredentials->EK, LoadedCredentials->KL);
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

void UAuthenticator::CallAuthSuccess(const FCredentials_BE& Credentials) const
{
	if (this->AuthSuccess.IsBound())
		this->AuthSuccess.Broadcast(Credentials);
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
					//UE_LOG(LogTemp,Display,TEXT("Token: %s"),*Token);
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
		NativeOAuth::SignInWithGoogle(FAuthenticatorConfig::GoogleClientID,this->Nonce,this);
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
#endif
	
#if PLATFORM_IOS
	FString clientID = FAuthenticatorConfig::UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type) + "&client_id=" + FAuthenticatorConfig::AppleClientID;
	switch (Type)
	{
	case ESocialSigninType::Apple:
		NativeOAuth::SignInWithApple(clientID, this->Nonce, this);
		break;
	case ESocialSigninType::Google:
		NativeOAuth::SignInWithGoogle_IOS(this->GetSigninURL(Type),FAuthenticatorConfig::UrlScheme,this);
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
#endif
}

FString UAuthenticator::GetSigninURL(const ESocialSigninType& Type)
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
	const FCredentials_BE Credentials(this->WaasSettings.GetRPCServer(), this->WaasSettings.GetProjectId(), FSequenceConfig::ProjectAccessKey,SessionPrivateKey,this->SessionId,this->Cached_IDToken,this->Cached_Email,FSequenceConfig::WaasVersion);
	this->StoreCredentials(Credentials);
	this->AutoRegister(Credentials);
}

void UAuthenticator::EmailLogin(const FString& EmailIn)
{
	if (this->CanHandleEmailLogin())
	{
		this->ResetRetryEmailLogin();
		this->Cached_Email = EmailIn;
		CognitoIdentityInitiateAuth(this->Cached_Email,this->WaasSettings.GetEmailClientId());
	}
	else
	{
		UE_LOG(LogTemp,Display,TEXT("Email based Auth not setup properly please ensure your WaasTenant key is configured properly"));
		this->CallAuthFailure();
	}
}

FString UAuthenticator::GenerateRedirectURL(const ESocialSigninType& Type) const
{
	FString RedirectUrl = FAuthenticatorConfig::RedirectURL + "&nonce=" + this->Nonce + "&scope=openid+email&state=" + FAuthenticatorConfig::UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
	switch (Type)
	{
	case ESocialSigninType::Google:
		break;
	case ESocialSigninType::Apple:
		RedirectUrl = FAuthenticatorConfig::RedirectURL;
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
	FString SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ FAuthenticatorConfig::RedirectURL + "&nonce=" + this->Nonce + "&scope=openid+email&state=" + FAuthenticatorConfig::UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
	switch (Type)
	{
	case ESocialSigninType::Google:
		break;
	case ESocialSigninType::Apple://For apple we have no scope, as well as the trailing response_mode
		SigninUrl = AuthUrl +"?response_type=code+id_token&client_id="+ AuthClientId +"&redirect_uri="+ FAuthenticatorConfig::RedirectURL + "&nonce=" + this->Nonce + "&state=" + FAuthenticatorConfig::UrlScheme + "---" + this->StateToken + UEnum::GetValueAsString(Type);
		SigninUrl += "&response_mode=fragment";
		break;
	case ESocialSigninType::FaceBook:
		break;
	case ESocialSigninType::Discord:
		break;
	}
	
	return SigninUrl;
}

FString UAuthenticator::BuildAWSURL(const FString& Service, const FString& AWSRegion)
{
	FString Ret = "https://"+ Service +"."+ AWSRegion +".amazonaws.com";
	return Ret;
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
		if (json.Get()->TryGetStringField("iss", iss))
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

FString UAuthenticator::ParseResponse(FHttpResponsePtr Response,bool WasSuccessful)
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

void UAuthenticator::ProcessCognitoIdentityInitiateAuth(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const FString response = this->ParseResponse(Response, bWasSuccessful);
	UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
	const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
	FString SessionPtr;
	if (responseObj->TryGetStringField("Session", SessionPtr))
	{//good state
		this->ChallengeSession = SessionPtr;
		this->CallAuthRequiresCode();
	}
	else
	{//error state
		if (response.Contains("user not found", ESearchCase::IgnoreCase))
		{//no user exists so create one!
			UE_LOG(LogTemp, Display, TEXT("Creating New User"));
			this->CognitoIdentitySignUp(this->Cached_Email, this->GenerateSignUpPassword(),this->WaasSettings.GetEmailClientId());
		}
		else
		{//unknown error
			UE_LOG(LogTemp, Error, TEXT("[Unexpected error when trying to authenticate]"));
			this->CallAuthFailure();
		}
	}
}

void UAuthenticator::CognitoIdentityInitiateAuth(const FString& Email, const FString& AWSCognitoClientID)
{
	const FString URL = BuildAWSURL("cognito-idp",this->WaasSettings.GetEmailRegion());
	const FString RequestBody = "{\"AuthFlow\":\"CUSTOM_AUTH\",\"AuthParameters\":{\"USERNAME\":\""+ Email +"\"},\"ClientId\":\""+ AWSCognitoClientID +"\"}";
	
	if (this->CanRetryEmailLogin())
	{
		this->UEAmazonWebServerRPC(URL, RequestBody,"AWSCognitoIdentityProviderService.InitiateAuth", &UAuthenticator::ProcessCognitoIdentityInitiateAuth);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Max login attempts reached]"));
		this->CallAuthFailure();
	}
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

void UAuthenticator::ProcessCognitoIdentitySignUp(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const FString response = this->ParseResponse(Response,bWasSuccessful);
	UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
	this->CognitoIdentityInitiateAuth(this->Cached_Email,this->WaasSettings.GetEmailClientId());
}

void UAuthenticator::CognitoIdentitySignUp(const FString& Email, const FString& Password, const FString& AWSCognitoClientID)
{
	const FString URL = BuildAWSURL("cognito-idp",this->WaasSettings.GetEmailRegion());
	const FString RequestBody = "{\"ClientId\":\""+ AWSCognitoClientID +"\",\"Password\":\""+ Password +"\",\"UserAttributes\":[{\"Name\":\"email\",\"Value\":\""+ Email +"\"}],\"Username\":\""+ Email +"\"}";

	this->UEAmazonWebServerRPC(URL,RequestBody, "AWSCognitoIdentityProviderService.SignUp",&UAuthenticator::ProcessCognitoIdentitySignUp);
}

void UAuthenticator::ProcessAdminRespondToAuthChallenge(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const FString response = this->ParseResponse(Response,bWasSuccessful);
	UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
	const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
	FString IDTokenPtr;
	const TSharedPtr<FJsonObject> *AuthObject;
	if (responseObj->TryGetObjectField("AuthenticationResult", AuthObject))
	{
		if (AuthObject->Get()->TryGetStringField("IdToken", IDTokenPtr))
		{//good state
			this->Cached_IDToken = IDTokenPtr;
			const FString SessionPrivateKey = BytesToHex(this->SessionWallet->GetWalletPrivateKey().Ptr(), this->SessionWallet->GetWalletPrivateKey().GetLength()).ToLower();
			const FCredentials_BE Credentials(this->WaasSettings.GetRPCServer(), this->WaasSettings.GetProjectId(), FSequenceConfig::ProjectAccessKey,SessionPrivateKey,this->SessionId,this->Cached_IDToken,this->Cached_Email,FSequenceConfig::WaasVersion);
			this->StoreCredentials(Credentials);
			this->AutoRegister(Credentials);
		}
		else
		{//error state
			UE_LOG(LogTemp, Error, TEXT("[No idToken found in AuthenticationResult]"));
			this->CallAuthFailure();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[No Authentication Result found]"));
		this->CallAuthFailure();
	}
}

void UAuthenticator::AdminRespondToAuthChallenge(const FString& Email, const FString& Answer, const FString& ChallengeSessionString, const FString& AWSCognitoClientID)
{
	const FString URL = BuildAWSURL("cognito-idp",this->WaasSettings.GetEmailRegion());
	const FString RequestBody = "{\"ChallengeName\":\"CUSTOM_CHALLENGE\",\"ClientId\":\""+ AWSCognitoClientID +"\",\"Session\":\""+ ChallengeSessionString +"\",\"ChallengeResponses\":{\"USERNAME\":\""+ Email +"\",\"ANSWER\":\""+ Answer +"\"},\"ClientMetadata\":{\"SESSION_HASH\":\""+this->SessionHash+"\"}}";
	this->UEAmazonWebServerRPC(URL, RequestBody,"AWSCognitoIdentityProviderService.RespondToAuthChallenge",&UAuthenticator::ProcessAdminRespondToAuthChallenge);
}

void UAuthenticator::AutoRegister(const FCredentials_BE& Credentials)
{
	USequenceWallet * Wallet = USequenceWallet::Make(Credentials);
	
	const TFunction<void (FString)> OnSuccess = [this](FString State)
	{
		FCredentials_BE CredData;
		FCredentials_BE * Cred = &CredData;
		if (GetStoredCredentials(Cred))
		{
			UE_LOG(LogTemp,Display,TEXT("Successfully Auto Registered Credentials: %s"),*State);
			this->CallAuthSuccess(*Cred);
		}
		else
		{
			UE_LOG(LogTemp,Display,TEXT("Failure During Auto Register: %s"),*State);
			this->CallAuthFailure();
		}
	};
	
	const TFunction<void (FSequenceError)> OnFailure = [this](FSequenceError Err)
	{
		UE_LOG(LogTemp,Display,TEXT("Failure During Auto Register: %s"),*Err.Message);
		this->CallAuthFailure();
	};
	
	Wallet->RegisterSession(OnSuccess,OnFailure);
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
	this->AdminRespondToAuthChallenge(this->Cached_Email,CodeIn,this->ChallengeSession,this->WaasSettings.GetEmailClientId());
}

FStoredCredentials_BE UAuthenticator::GetStoredCredentials() const
{
	FCredentials_BE CredData;
	FCredentials_BE* Credentials = &CredData;
	const bool IsValid = this->GetStoredCredentials(Credentials);
	return FStoredCredentials_BE(IsValid, *Credentials);
}

void UAuthenticator::UEAmazonWebServerRPC(const FString& Url, const FString& RequestBody,const FString& AMZTarget,void(UAuthenticator::*Callback)(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful))
{
	UE_LOG(LogTemp,Display,TEXT("RequestBody: %s"),*RequestBody);
	const TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetURL(Url);
	http_post_req->SetHeader("Content-type", "application/x-amz-json-1.1");
	http_post_req->SetHeader("X-AMZ-TARGET", AMZTarget);
	http_post_req->SetContentAsString(RequestBody);
	http_post_req->SetTimeout(15);
	http_post_req->OnProcessRequestComplete().BindUObject(this,Callback);
	http_post_req->ProcessRequest();
}