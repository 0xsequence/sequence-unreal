// Fill out your copyright notice in the Description page of Project Settings.

#include "Authenticator.h"
#include "HttpModule.h"
#include "Misc/Guid.h"
#include "AES/aes.c"
#include "AES/aes.h"
#include "Misc/Base64.h"
#include "Eth/EthTransaction.h"
#include "Types/Wallet.h"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256.hpp"
#include "Util/HexUtility.h"
#include "StorableCredentials.h"
#include "Kismet/GameplayStatics.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceEncryptor.h"
#include "IWebBrowserCookieManager.h"
#include "IWebBrowserSingleton.h"
#include "WebBrowserModule.h"
#include "Core/Tests/Containers/TestUtils.h"
#include "Interfaces/IHttpResponse.h"

UAuthenticator::UAuthenticator()
{
	this->SessionWallet = UWallet::Make();//Generate a new Random UWallet
	this->Nonce = "0x" + this->SessionWallet->GetWalletAddress().ToHex().ToLower();
	this->StateToken = FGuid::NewGuid().ToString();
	FString ParsedJWT;
	FBase64::Decode(this->VITE_SEQUENCE_WAAS_CONFIG_KEY,ParsedJWT);
	UE_LOG(LogTemp, Display, TEXT("Decoded Data: %s"),*ParsedJWT);
	this->WaasCredentials = FWaasCredentials(UIndexerSupport::jsonStringToStruct<FWaasJWT>(ParsedJWT));
	
}

void UAuthenticator::StoreCredentials(const FCredentials_BE& Credentials) const
{
	if (UStorableCredentials* StorableCredentials = Cast<UStorableCredentials>(UGameplayStatics::CreateSaveGameObject(UStorableCredentials::StaticClass())))
	{
		const FString CTS_Json = UIndexerSupport::structToString<FCredentials_BE>(Credentials);
		const int32 CTS_Json_Length = CTS_Json.Len();

		StorableCredentials->EK = USequenceEncryptor::Encrypt(CTS_Json);
		StorableCredentials->KL = CTS_Json_Length;

		if (UGameplayStatics::SaveGameToSlot(StorableCredentials, this->SaveSlot, this->UserIndex))
		{
			UE_LOG(LogTemp, Display, TEXT("Data Saved Correctly!"));
		}
	}
}

bool UAuthenticator::GetStoredCredentials(FCredentials_BE* Credentials) const
{
	bool ret = false;
	if (const UStorableCredentials* LoadedCredentials = Cast<UStorableCredentials>(UGameplayStatics::LoadGameFromSlot(this->SaveSlot, this->UserIndex)))
	{
		const FString CTR_Json = USequenceEncryptor::Decrypt(LoadedCredentials->EK, LoadedCredentials->KL);
		ret = UIndexerSupport::jsonStringToStruct<FCredentials_BE>(CTR_Json, Credentials);
		ret &= this->CredentialsValid(*Credentials);
	}
	return ret;
}

bool UAuthenticator::CredentialsValid(const FCredentials_BE& Credentials)
{
	return FDateTime::UtcNow().ToUnixTimestamp() < Credentials.GetExpires();
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

FString UAuthenticator::GetSigninURL(const ESocialSigninType& Type) const
{
	FString SigninURL = "";
	
	if (this->SSOProviderMap.Contains(Type))
	{
		SigninURL = this->GenerateSigninURL(this->SSOProviderMap[Type].URL, this->SSOProviderMap[Type].ClientID);
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

FString UAuthenticator::GetRedirectURL() const
{
	UE_LOG(LogTemp,Display,TEXT(""));
	UE_LOG(LogTemp,Display,TEXT("RedirectURL: %s"),*this->RedirectURL);
	UE_LOG(LogTemp,Display,TEXT("Padder: %s"),*this->BRedirectURL);
	return this->RedirectURL;
}

void UAuthenticator::SocialLogin(const FString& IDTokenIn)
{
	this->Cached_IDToken = IDTokenIn;
	CognitoIdentityGetID(this->WaasCredentials.GetIdentityPoolId(),GetISSClaim(this->Cached_IDToken), this->Cached_IDToken);
}

void UAuthenticator::EmailLogin(const FString& EmailIn)
{
	this->ResetRetryEmailLogin();
	this->Cached_Email = EmailIn;
	CognitoIdentityInitiateAuth(this->Cached_Email,this->WaasCredentials.GetCognitoClientId());
}

FString UAuthenticator::GenerateSigninURL(const FString& AuthURL, const FString& ClientID) const
{
	//return FString::Printf(TEXT("%s?response_type=id_token&client_id=%s&redirect_uri=%s&scope=openid+profile+email&state=%s&nonce=%s"),*AuthURL,*ClientID,*this->RedirectURL,*this->StateToken, *this->Nonce);
	return AuthURL +"?response_type=id_token&client_id="+ ClientID +"&redirect_uri="+ this->RedirectURL +"&scope=openid+profile+email&state="+ this->StateToken +"&nonce="+ this->Nonce;
}

FString UAuthenticator::BuildAWSURL(const FString& Service, const FString& AWSRegion)
{
	//return FString::Printf(TEXT("https://%s.%s.amazonaws.com"),*Service,*AWSRegion);
	UE_LOG(LogTemp,Display,TEXT("(In build AWS URL) Service: %s"), *Service);
	UE_LOG(LogTemp,Display,TEXT("(In build AWS URL) Region: %s"), *AWSRegion);
	FString Ret = "https://"+ Service +"."+ AWSRegion +".amazonaws.com";
	UE_LOG(LogTemp, Display, TEXT("AWSURL: %s"), *Ret);
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

void UAuthenticator::ProcessCognitoGetID(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const FString response = this->ParseResponse(Response,bWasSuccessful);
	UE_LOG(LogTemp, Display, TEXT("Response %s"),*response);
	const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
	FString IdentityIdPtr = "nil";
	if (responseObj->TryGetStringField("IdentityId",IdentityIdPtr))
	{//good state
		this->IdentityId = IdentityIdPtr;
		CognitoIdentityGetCredentialsForIdentity(this->IdentityId, this->Cached_IDToken, this->Cached_Issuer);
	}
	else
	{//error state
		UE_LOG(LogTemp, Error, TEXT("[No IdentityID found in AWS Response]"));
		this->CallAuthFailure();
	}
}

void UAuthenticator::CognitoIdentityGetID(const FString& PoolID,const FString& Issuer, const FString& IDToken)
{
	PrintAll();
	this->Cached_Issuer = Issuer;
	const FString URL = this->BuildAWSURL("cognito-identity",this->WaasCredentials.GetIDPRegion());
	const FString RequestBody = "{\"IdentityPoolId\":\""+ PoolID +"\",\"Logins\":{\""+ Issuer +"\":\""+ IDToken +"\"}}";
	this->UE_AWS_RPC(URL,RequestBody,"com.amazonaws.cognito.identity.model.AWSCognitoIdentityService.GetId",&UAuthenticator::ProcessCognitoGetID);
}

void UAuthenticator::ProcessCognitoIdentityGetCredentialsForIdentity(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const FString response = this->ParseResponse(Response, bWasSuccessful);
	UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
	const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
	const TSharedPtr<FJsonObject> *credObj;

	if (responseObj->TryGetObjectField("Credentials", credObj))
	{
		FString AccessKeyPtr, SecretKeyPtr, SessionTokenPtr;
		if (credObj->Get()->TryGetStringField("AccessKeyId", AccessKeyPtr) && credObj->Get()->TryGetStringField("SecretKey", SecretKeyPtr) && credObj->Get()->TryGetStringField("SessionToken", SessionTokenPtr))
		{//good state
			this->AccessKeyId = AccessKeyPtr;
			this->SecretKey = SecretKeyPtr;
			this->SessionToken = SessionTokenPtr;
			this->KMSGenerateDataKey(this->WaasCredentials.GetKMSArn());
		}
		else
		{//error state
			UE_LOG(LogTemp, Error, TEXT("[Missing Credentials in AWS Response]"));
			this->CallAuthFailure();
		}
	}
	else
	{//error state
		UE_LOG(LogTemp, Error, TEXT("[Missing Credentials in AWS Response]"));
		this->CallAuthFailure();
	}
}

void UAuthenticator::CognitoIdentityGetCredentialsForIdentity(const FString& IdentityID, const FString& IDToken, const FString& Issuer)
{
	PrintAll();
	const FString URL = this->BuildAWSURL("cognito-identity",this->WaasCredentials.GetIDPRegion());
	const FString RequestBody = "{\"IdentityId\":\""+ IdentityID +"\",\"Logins\":{\""+ Issuer +"\":\""+ IDToken +"\"}}";
	this->UE_AWS_RPC(URL,RequestBody,"AWSCognitoIdentityService.GetCredentialsForIdentity",&UAuthenticator::ProcessCognitoIdentityGetCredentialsForIdentity);
}

FString UAuthenticator::BuildYYYYMMDD(const FDateTime& Date)
{
	return Date.ToString(TEXT("%Y%m%d"));//YYYYMMDD
}

FString UAuthenticator::BuildFullDateTime(const FDateTime& Date)
{
	return Date.ToString(TEXT("%Y%m%dT%H%M%SZ"));//YYYYMMDDTHHMMSSZ
}

FString UAuthenticator::BuildScope(const FDateTime& Date)
{
	return BuildYYYYMMDD(Date) + "/" + this->WaasCredentials.GetKMSRegion() + "/kms/aws4_request";
}

FString UAuthenticator::BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload)
{
	const FUnsizedData PayloadBytes = StringToUTF8(Payload);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Ptr(), PayloadBytes.GetLength());

	const FString Verb = "POST";
	const FString CanonicalURI = "/";//we have none
	const FString CanQueryString = "";//no query parameters so leave this blank
	const FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();
	const FString CanHeaders = "content-type:application/x-amz-json-1.1\nhost:" + URI + "\nx-amz-content-sha256:" + HashPayload + "\nx-amz-date:" + BuildFullDateTime(Date) + "\nx-amz-target:TrentService.GenerateDataKey\n";
	const FString SignedHeaders = "content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target";

	//build the CanonicalRequest
	FString CanReq = Verb + "\n" + CanonicalURI + "\n" + CanQueryString + "\n" + CanHeaders + "\n" + SignedHeaders + "\n" + HashPayload;

	return CanReq;
}

/*
* 
*/
FString UAuthenticator::BuildStringToSign(const FDateTime& Date, const FString& CanonicalRequest)
{
	const FUnsizedData PayloadBytes = StringToUTF8(CanonicalRequest);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Ptr(), PayloadBytes.GetLength());

	const FString CanonicalHash = BytesToHex(PayloadHashBytes.value,PayloadHashBytes.HASH_LEN);
	const FString FullDate = BuildFullDateTime(Date);
	FString StringToSign = "AWS4-HMAC-SHA256\n" + FullDate + "\n" + this->BuildScope(Date) + "\n" + CanonicalHash.ToLower();

	const FUnsizedData StringToSignBytes = StringToUTF8(StringToSign);
	const Sha256Hash StringToSignHashBytes = Sha256::getHash(StringToSignBytes.Ptr(), StringToSignBytes.GetLength());
	FString StringToSignHash = BytesToHex(StringToSignHashBytes.value, StringToSignHashBytes.HASH_LEN).ToLower();

	return StringToSign;
}

TArray<uint8_t> UAuthenticator::BuildSigningKey(const FDateTime& Date)
{
	const FUnsizedData DateKeyEncryptBytes = StringToUTF8("AWS4" + this->SecretKey);
	const FUnsizedData TimeStampBytes = StringToUTF8(BuildYYYYMMDD(Date));
	const FUnsizedData RegionBytes = StringToUTF8(this->WaasCredentials.GetKMSRegion());
	const FUnsizedData KMSBytes = StringToUTF8("kms");
	const FUnsizedData AWSReqBytes = StringToUTF8("aws4_request");

	const Sha256Hash DateKeyHash = Sha256::getHmac(DateKeyEncryptBytes.Ptr(),DateKeyEncryptBytes.GetLength(),TimeStampBytes.Ptr(),TimeStampBytes.GetLength());
	const Sha256Hash DateRegionKeyHash = Sha256::getHmac(DateKeyHash.value, DateKeyHash.HASH_LEN, RegionBytes.Ptr(), RegionBytes.GetLength());
	const Sha256Hash DateRegionServiceKeyHash = Sha256::getHmac(DateRegionKeyHash.value, DateRegionKeyHash.HASH_LEN, KMSBytes.Ptr(), KMSBytes.GetLength());
	const Sha256Hash SigningKeyHash = Sha256::getHmac(DateRegionServiceKeyHash.value, DateRegionServiceKeyHash.HASH_LEN, AWSReqBytes.Ptr(), AWSReqBytes.GetLength());

	FString DateKey = BytesToHex(DateKeyHash.value, DateKeyHash.HASH_LEN).ToLower();
	FString DateRegionKey = BytesToHex(DateRegionKeyHash.value,DateRegionKeyHash.HASH_LEN).ToLower();
	FString DateRegionServiceKey = BytesToHex(DateRegionServiceKeyHash.value, DateRegionServiceKeyHash.HASH_LEN).ToLower();
	FString SigningKey = BytesToHex(SigningKeyHash.value, SigningKeyHash.HASH_LEN).ToLower();

	TArray<uint8_t> SigningBytes;
	SigningBytes.Append(SigningKeyHash.value,SigningKeyHash.HASH_LEN);

	return SigningBytes;
}

FString UAuthenticator::BuildSignature(const TArray<uint8_t>& SigningKey, const FString& StringToSign)
{
	const FUnsizedData StringToSignBytes = StringToUTF8(StringToSign);
	const Sha256Hash SignatureHash = Sha256::getHmac(SigningKey.GetData(),SigningKey.Num(), StringToSignBytes.Ptr(), StringToSignBytes.GetLength());
	FString Signature = BytesToHex(SignatureHash.value,SignatureHash.HASH_LEN).ToLower();

	return Signature;
}

FString UAuthenticator::BuildKMSAuthorizationHeader(const FDateTime& Date, const FString& URI, const FString& Payload)
{
	FString AuthHeader = "AWS4-HMAC-SHA256 Credential=";
	const FString dateString = this->BuildYYYYMMDD(Date);
	const FString CanReq = this->BuildCanonicalRequest(URI, Date, Payload);
	const FString StringToSign = this->BuildStringToSign(Date, CanReq);
	const TArray<uint8_t> SigningKey = this->BuildSigningKey(Date);
	const FString Signature = this->BuildSignature(SigningKey, StringToSign);
	//AuthHeader += FString::Printf(TEXT("%s"), *this->AccessKeyId) + TEXT("/") + FString::Printf(TEXT("%s"), *dateString) + TEXT("/") + FString::Printf(TEXT("%s"), *this->Region) + TEXT("/") + FString::Printf(TEXT("%s"), *this->AWSService) + TEXT("/aws4_request,SignedHeaders=content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target,Signature=") + FString::Printf(TEXT("%s"), *Signature);
	AuthHeader += this->AccessKeyId + "/" + dateString + "/" + this->WaasCredentials.GetKMSRegion() + "/kms/aws4_request,SignedHeaders=content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target,Signature=" + Signature;
	return AuthHeader;
}

void UAuthenticator::ProcessKMSGenerateDataKey(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString response = this->ParseResponse(Response,bWasSuccessful);
	UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
	const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
	FString PlainTextPtr, CipherTextBlobPtr;
	if (responseObj->TryGetStringField("Plaintext", PlainTextPtr) && responseObj->TryGetStringField("CipherTextBlob", CipherTextBlobPtr))
	{//good state
		TArray<uint8> PlainTextBytes_cached;
		FBase64::Decode(PlainTextPtr, PlainTextBytes_cached);
		this->PlainText = BytesToHex(PlainTextBytes_cached.GetData(),PlainTextBytes_cached.Num()).ToLower();
		this->PlainTextBytes = PlainTextBytes_cached;

		TArray<uint8> CipherTextBytes;
		FBase64::Decode(CipherTextBlobPtr, CipherTextBytes);
		this->CipherTextBlob = BytesToHex(CipherTextBytes.GetData(), CipherTextBytes.Num()).ToLower();
		this->AuthWithSequence(this->Cached_IDToken,PlainTextBytes);
	}
	else
	{//error state
		UE_LOG(LogTemp, Error, TEXT("[Missing Credentials in AWS Response]"));
		this->CallAuthFailure();
	}
}

void UAuthenticator::KMSGenerateDataKey(const FString& AWSKMSKeyID)
{
	PrintAll();
	const FString URL = BuildAWSURL("kms",this->WaasCredentials.GetKMSRegion());
	const FString RequestBody = "{\"KeyId\":\""+ AWSKMSKeyID +"\",\"KeySpec\":\"AES_256\"}";
	const FDateTime cachedDate = FDateTime::UtcNow();
	this->UE_AWS_AuthorizedRPC(this->BuildKMSAuthorizationHeader(cachedDate,URL.RightChop(8), RequestBody),cachedDate, URL, "TrentService.GenerateDataKey", RequestBody,&UAuthenticator::ProcessKMSGenerateDataKey);
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
			this->CognitoIdentitySignUp(this->Cached_Email, this->GenerateSignUpPassword(),this->WaasCredentials.GetCognitoClientId());
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
	PrintAll();
	const FString URL = BuildAWSURL("cognito-idp",this->WaasCredentials.GetIDPRegion());
	const FString RequestBody = "{\"AuthFlow\":\"CUSTOM_AUTH\",\"AuthParameters\":{\"USERNAME\":\""+ Email +"\"},\"ClientId\":\""+ AWSCognitoClientID +"\"}";
	
	if (this->CanRetryEmailLogin())
	{
		this->UE_AWS_RPC(URL, RequestBody,"AWSCognitoIdentityProviderService.InitiateAuth", &UAuthenticator::ProcessCognitoIdentityInitiateAuth);
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
	this->CognitoIdentityInitiateAuth(this->Cached_Email,this->WaasCredentials.GetCognitoClientId());
}

void UAuthenticator::CognitoIdentitySignUp(const FString& Email, const FString& Password, const FString& AWSCognitoClientID)
{
	PrintAll();
	const FString URL = BuildAWSURL("cognito-idp",this->WaasCredentials.GetIDPRegion());
	const FString RequestBody = "{\"ClientId\":\""+ AWSCognitoClientID +"\",\"Password\":\""+ Password +"\",\"UserAttributes\":[{\"Name\":\"email\",\"Value\":\""+ Email +"\"}],\"Username\":\""+ Email +"\"}";

	this->UE_AWS_RPC(URL,RequestBody, "AWSCognitoIdentityProviderService.SignUp",&UAuthenticator::ProcessCognitoIdentitySignUp);
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
			this->SocialLogin(this->Cached_IDToken);
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
	PrintAll();
	const FString URL = BuildAWSURL("cognito-idp",this->WaasCredentials.GetIDPRegion());
	const FString RequestBody = "{\"ChallengeName\":\"CUSTOM_CHALLENGE\",\"ClientId\":\""+ AWSCognitoClientID +"\",\"Session\":\""+ ChallengeSessionString +"\",\"ChallengeResponses\":{\"USERNAME\":\""+ Email +"\",\"ANSWER\":\""+ Answer +"\"}}";
	this->UE_AWS_RPC(URL, RequestBody,"AWSCognitoIdentityProviderService.RespondToAuthChallenge",&UAuthenticator::ProcessAdminRespondToAuthChallenge);
}

TSharedPtr<FJsonObject> UAuthenticator::ResponseToJson(const FString& response)
{
	TSharedPtr<FJsonObject> responseObj;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(response), responseObj))
		return responseObj;
	else
		return nullptr;
}

void UAuthenticator::EmailLoginCode(const FString& CodeIn)
{
	this->AdminRespondToAuthChallenge(this->Cached_Email,CodeIn,this->ChallengeSession,this->WaasCredentials.GetCognitoClientId());
}

FStoredCredentials_BE UAuthenticator::GetStoredCredentials() const
{
	FCredentials_BE CredData;
	FCredentials_BE* Credentials = &CredData;
	const bool IsValid = this->GetStoredCredentials(Credentials);
	return FStoredCredentials_BE(IsValid, *Credentials);
}

void UAuthenticator::PrintAll()
{
	const FString creds = UIndexerSupport::structToString<FWaasCredentials>(this->WaasCredentials);
	UE_LOG(LogTemp,Display,TEXT("FWaasCredentials: %s"), *creds);
	FString PurgeCacheString = (this->PurgeCache) ? "true" : "false";
	UE_LOG(LogTemp,Display,TEXT("PurgeCache: %s"), *PurgeCacheString);
	UE_LOG(LogTemp,Display,TEXT("ChallengeSession: %s"), *this->ChallengeSession);
	UE_LOG(LogTemp,Display,TEXT("CipherTextBlob: %s"), *this->CipherTextBlob);
	UE_LOG(LogTemp,Display,TEXT("PlainText: %s"), *this->PlainText);
	UE_LOG(LogTemp,Display,TEXT("SessionToken: %s"), *this->SessionToken);
	UE_LOG(LogTemp,Display,TEXT("SecretKey: %s"), *this->SecretKey);
	UE_LOG(LogTemp,Display,TEXT("SecretKey: %s"), *this->SecretKey);
	UE_LOG(LogTemp,Display,TEXT("AccessKeyId: %s"), *this->AccessKeyId);
	UE_LOG(LogTemp,Display,TEXT("IdentityId: %s"), *this->IdentityId);
	UE_LOG(LogTemp,Display,TEXT("EmailAuthMaxRetries: %d"), this->EmailAuthMaxRetries);
	UE_LOG(LogTemp,Display,TEXT("EmailAuthCurrRetries: %d"), this->EmailAuthCurrRetries);
	UE_LOG(LogTemp,Display,TEXT("WaasVersion: %s"), *this->WaasVersion);
	UE_LOG(LogTemp,Display,TEXT("ProjectAccessKey: %s"), *this->ProjectAccessKey);
	UE_LOG(LogTemp,Display,TEXT("VITE_SEQUENCE_WAAS_CONFIG_KEY: %s"), *this->VITE_SEQUENCE_WAAS_CONFIG_KEY);
	UE_LOG(LogTemp,Display,TEXT("cached_email: %s"), *this->Cached_Email);
	UE_LOG(LogTemp,Display,TEXT("cachedidtoken: %s"), *this->Cached_IDToken);
	UE_LOG(LogTemp,Display,TEXT("appleclientid: %s"), *this->AppleClientID);
	UE_LOG(LogTemp,Display,TEXT("appleauthurl: %s"), *this->AppleAuthURL);
	UE_LOG(LogTemp,Display,TEXT("discordclientid: %s"), *this->DiscordClientID);
	UE_LOG(LogTemp,Display,TEXT("discordurl: %s"), *this->DiscordAuthURL);
	UE_LOG(LogTemp,Display,TEXT("facebookclientid: %s"), *this->FacebookClientID);
	UE_LOG(LogTemp,Display,TEXT("facebookurl: %s"), *this->FacebookAuthURL);
	UE_LOG(LogTemp,Display,TEXT("googleclientid: %s"), *this->GoogleClientID);
	UE_LOG(LogTemp,Display,TEXT("googleauthurl: %s"), *this->GoogleAuthURL);
	UE_LOG(LogTemp,Display,TEXT("bredirecturl: %s"), *this->BRedirectURL);
	UE_LOG(LogTemp,Display,TEXT("redirecturl: %s"), *this->RedirectURL);
	UE_LOG(LogTemp,Display,TEXT("urlscheme: %s"), *this->UrlScheme);
	UE_LOG(LogTemp,Display,TEXT("nonce: %s"), *this->Nonce);
	UE_LOG(LogTemp,Display,TEXT("statetoken: %s"), *this->StateToken);
	UE_LOG(LogTemp,Display,TEXT("saveslot: %s"), *this->SaveSlot);
	UE_LOG(LogTemp,Display,TEXT("UserIndex 0 : %d"), this->UserIndex);
}

void UAuthenticator::ProcessAuthWithSequence(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful)
{
	const FString response = this->ParseResponse(Response, bWasSuccessful);
			UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		const TSharedPtr<FJsonObject>* SessionObj = nullptr;
		const TSharedPtr<FJsonObject>* DataObj = nullptr;
		FString PlainTextPtr, CipherTextBlobPtr;
		if (responseObj->TryGetObjectField("session", SessionObj) && responseObj->TryGetObjectField("data", DataObj))
		{//good state
			FString Id, Address, UserId, Subject, SessionId, Wallet, Issuer;
			FString Issued, Refreshed, Expires;
			if (SessionObj->Get()->TryGetStringField("issuer",Issuer) && 
				SessionObj->Get()->TryGetStringField("id", Id) && 
				SessionObj->Get()->TryGetStringField("address", Address) && 
				SessionObj->Get()->TryGetStringField("userId", UserId) && 
				SessionObj->Get()->TryGetStringField("subject", Subject) && 
				SessionObj->Get()->TryGetStringField("createdAt",Issued) &&
				SessionObj->Get()->TryGetStringField("refreshedAt",Refreshed) &&
				SessionObj->Get()->TryGetStringField("expiresAt",Expires) &&
				DataObj->Get()->TryGetStringField("sessionId", SessionId) && 
				DataObj->Get()->TryGetStringField("wallet", Wallet))
			{
				FDateTime IssuedDT, RefreshedDT, ExpiresDT;
				FDateTime::ParseIso8601(*Issued, IssuedDT);
				FDateTime::ParseIso8601(*Refreshed, RefreshedDT);
				FDateTime::ParseIso8601(*Expires, ExpiresDT);
				const FString SessionPrivateKey = BytesToHex(this->SessionWallet->GetWalletPrivateKey().Ptr(), this->SessionWallet->GetWalletPrivateKey().GetLength()).ToLower();
				const FCredentials_BE Credentials(this->WaasCredentials.GetProjectID(),this->PlainTextBytes, this->CipherTextBlob, this->ProjectAccessKey , SessionPrivateKey, Id, Address, UserId, Subject, SessionId, Wallet, this->Cached_IDToken, this->Cached_Email, Issuer,IssuedDT.ToUnixTimestamp(), RefreshedDT.ToUnixTimestamp(), ExpiresDT.ToUnixTimestamp(),this->WaasVersion);
				this->StoreCredentials(Credentials);
				this->CallAuthSuccess(Credentials);
			}
			else
			{//error state
				UE_LOG(LogTemp, Error, TEXT("[Missing Credentials in Sequence Session Response]"));
				this->CallAuthFailure();
			}
		}
		else
		{//error state
			UE_LOG(LogTemp, Error, TEXT("[Missing Credentials in Sequence Response]"));
			this->CallAuthFailure();
		}
}

void UAuthenticator::AuthWithSequence(const FString& IDTokenIn, const TArray<uint8_t>& Key)
{
	PrintAll();
	int64 UnixIssueTime = FDateTime::UtcNow().ToUnixTimestamp();
	int64 UnixExpireTime = UnixIssueTime + 30;
	FString UnixIssueString = FString::Printf(TEXT("%lld"), UnixIssueTime);
	FString UnixExpireString = FString::Printf(TEXT("%lld"), UnixExpireTime);
	FString CachedWalletAddress = FString::Printf(TEXT("0x%s"),*BytesToHex(this->SessionWallet->GetWalletAddress().Ptr(), this->SessionWallet->GetWalletAddress().GetLength()).ToLower());
	FString Intent = "{\\\"version\\\":\\\""+ this->WaasVersion +"\\\",\\\"packet\\\":{\\\"code\\\":\\\"openSession\\\",\\\"expires\\\":"+ UnixExpireString +",\\\"issued\\\":"+ UnixIssueString +",\\\"session\\\":\\\""+ CachedWalletAddress +"\\\",\\\"proof\\\":{\\\"idToken\\\":\\\""+ IDTokenIn +"\\\"}}}";
	FString Payload = "{\"projectId\":"+ this->WaasCredentials.GetProjectID() +",\"idToken\":\""+ IDTokenIn +"\",\"sessionAddress\":\""+ CachedWalletAddress +"\",\"friendlyName\":\"FRIENDLY SESSION WALLET\",\"intentJSON\":\""+ Intent +"\"}";
	
	UE_LOG(LogTemp, Display, TEXT("Payload: %s"), *Payload);

	TArray<uint8_t> TPayload = PKCS7(Payload);
	AES_ctx ctx;
	struct AES_ctx* PtrCtx = &ctx;

	constexpr int32 IVSize = 16;
	TArray<uint8_t> iv;

	for (int i = 0; i < IVSize; i++)
		iv.Add(RandomByte());

	AES_init_ctx_iv(PtrCtx, Key.GetData(), iv.GetData());
	AES_CBC_encrypt_buffer(PtrCtx, TPayload.GetData(), TPayload.Num());

	FString PrePendedKey = BytesToHex(Key.GetData(),Key.Num());
	FString PrePendedIV = BytesToHex(iv.GetData(),iv.Num());
	FString PrePendedCipher = BytesToHex(TPayload.GetData(), TPayload.Num());
	FString PublicKey = BytesToHex(this->SessionWallet->GetWalletPublicKey().Ptr(),this->SessionWallet->GetWalletPublicKey().GetLength()).ToLower();
	FString PrivateKey = BytesToHex(this->SessionWallet->GetWalletPrivateKey().Ptr(),this->SessionWallet->GetWalletPrivateKey().GetLength()).ToLower();
	FString Address = BytesToHex(this->SessionWallet->GetWalletAddress().Ptr(),this->SessionWallet->GetWalletAddress().GetLength()).ToLower();
	FString PayloadCipherText = "0x" + BytesToHex(iv.GetData(), iv.Num()).ToLower() + BytesToHex(TPayload.GetData(), TPayload.Num()).ToLower();
	
	TArray<uint8_t> PayloadSigBytes = this->SessionWallet->SignMessage(Payload);
	
	FString PayloadSig = "0x" + BytesToHex(PayloadSigBytes.GetData(), PayloadSigBytes.Num()).ToLower();
	FString EncryptedPayloadKey = "0x" + this->CipherTextBlob;
	FString FinalPayload = "{\"payloadSig\":\""+ PayloadSig +"\",\"encryptedPayloadKey\":\""+ EncryptedPayloadKey +"\",\"payloadCiphertext\":\""+ PayloadCipherText +"\"}";

	this->UE_SequenceRPC(this->WaasCredentials.GetRPCServer(), FinalPayload,&UAuthenticator::ProcessAuthWithSequence);
}

void UAuthenticator::UE_AWS_AuthorizedRPC(const FString& Authorization, const FDateTime& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody,void(UAuthenticator::*Callback)(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful))
{
	const FUnsizedData PayloadBytes = StringToUTF8(RequestBody);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Ptr(), PayloadBytes.GetLength());
	const FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();
	
	const TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetURL(Url);
	http_post_req->SetHeader("Host", Url.RightChop(8));
	http_post_req->SetHeader("Content-type", "application/x-amz-json-1.1");
	http_post_req->SetHeader("X-AMZ-TARGET", AMZTarget);
	http_post_req->SetHeader("X-AMZ-DATE", BuildFullDateTime(Date));
	http_post_req->SetHeader("X-AMZ-CONTENT-SHA256" , HashPayload);
	http_post_req->SetHeader("x-amz-security-token", this->SessionToken);
	http_post_req->SetHeader("Authorization", Authorization);
	http_post_req->SetContentAsString(RequestBody);
	http_post_req->SetTimeout(15);
	http_post_req->OnProcessRequestComplete().BindUObject(this,Callback);
	http_post_req->ProcessRequest();
}

void UAuthenticator::UE_SequenceRPC(const FString& Url, const FString& RequestBody, void(UAuthenticator::*Callback)(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful))
{
	const TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetURL(Url);
	http_post_req->SetHeader("Content-type", "application/json");
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetHeader("X-Access-Key", this->ProjectAccessKey);
	http_post_req->SetContentAsString(RequestBody);
	http_post_req->SetTimeout(15);
	http_post_req->OnProcessRequestComplete().BindUObject(this,Callback);
	http_post_req->ProcessRequest();
	
}

void UAuthenticator::UE_AWS_RPC(const FString& Url, const FString& RequestBody,const FString& AMZTarget,void(UAuthenticator::*Callback)(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful))
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