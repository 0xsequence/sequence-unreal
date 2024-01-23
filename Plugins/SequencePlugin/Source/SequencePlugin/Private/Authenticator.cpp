// Fill out your copyright notice in the Description page of Project Settings.

#include "Authenticator.h"
#include "Misc/Guid.h"
#include "AES/aes.c"
#include "AES/aes.h"
#include "Eth/EthTransaction.h"
#include "Types/Wallet.h"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256.hpp"
#include "Util/HexUtility.h"
#include "StorableCredentials.h"
#include "Kismet/GameplayStatics.h"
#include "Indexer/IndexerSupport.h"
#include "SequenceEncryptor.h"

UAuthenticator::UAuthenticator()
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
	FString ParsedJWT;
	FBase64::Decode(this->VITE_SEQUENCE_WAAS_CONFIG_KEY,ParsedJWT);
	this->WaasCredentials = FWaasCredentials(UIndexerSupport::jsonStringToStruct<FWaasJWT>(ParsedJWT));
}

void UAuthenticator::StoreCredentials(const FCredentials_BE& Credentials)
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

bool UAuthenticator::GetStoredCredentials(FCredentials_BE* Credentials)
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

void UAuthenticator::CallAuthRequiresCode()
{
	if (this->AuthRequiresCode.IsBound())
		this->AuthRequiresCode.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Failure: nothing bound to delegate: AuthRequiresCode]"));
}

void UAuthenticator::CallAuthFailure()
{
	if (this->AuthFailure.IsBound())
		this->AuthFailure.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: AuthFailure]"));
}

void UAuthenticator::CallAuthSuccess(const FCredentials_BE& Credentials)
{
	if (this->AuthSuccess.IsBound())
		this->AuthSuccess.Broadcast(Credentials);
	else
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: AuthSuccess]"));
}

FString UAuthenticator::GetSigninURL(const ESocialSigninType& Type)
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

	return SigninURL;
}

FString UAuthenticator::GetRedirectURL()
{
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

FString UAuthenticator::GenerateSigninURL(const FString& AuthURL, const FString& ClientID)
{
	//return FString::Printf(TEXT("%s?response_type=id_token&client_id=%s&redirect_uri=%s&scope=openid+profile+email&state=%s&nonce=%s"),*AuthURL,*ClientID,*this->RedirectURL,*this->StateToken, *this->Nonce);
	return AuthURL +"?response_type=id_token&client_id="+ ClientID +"&redirect_uri="+ this->RedirectURL +"&scope=openid+profile+email&state="+ this->StateToken +"&nonce="+ this->Nonce;
}

FString UAuthenticator::BuildAWSURL(const FString& Service, const FString& AWSRegion)
{
	//return FString::Printf(TEXT("https://%s.%s.amazonaws.com"),*Service,*AWSRegion);
	return "https://"+ Service +"."+ AWSRegion +".amazonaws.com";
}

FString UAuthenticator::GetISSClaim(const FString& JWT)
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
		TSharedPtr<FJsonObject> json = this->ResponseToJson(B64Json[1]);
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

void UAuthenticator::CognitoIdentityGetID(const FString& PoolID,const FString& Issuer, const FString& IDToken)
{
	const FString URL = this->BuildAWSURL("cognito-identity",this->WaasCredentials.GetIDPRegion());
	//FString RequestBody = FString::Printf(TEXT("{\"IdentityPoolId\":\"%s\",\"Logins\":{\"%s\":\"%s\"}}"),*PoolID,*Issuer, *IDToken);
	const FString RequestBody = "{\"IdentityPoolId\":\""+ PoolID +"\",\"Logins\":{\""+ Issuer +"\":\""+ IDToken +"\"}}";
	
	const TSuccessCallback<FString> GenericSuccess = [this,Issuer](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"),*response);
		const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString IdentityIdPtr = "nil";
		if (responseObj->TryGetStringField("IdentityId",IdentityIdPtr))
		{//good state
			this->IdentityId = IdentityIdPtr;
			CognitoIdentityGetCredentialsForIdentity(this->IdentityId, this->Cached_IDToken, Issuer);
		}
		else
		{//error state
			UE_LOG(LogTemp, Error, TEXT("[No IdentityID found in AWS Response]"));
			this->CallAuthFailure();
		}
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"),*Error.Message);
		this->CallAuthFailure();
	};

	this->RPC(URL,"com.amazonaws.cognito.identity.model.AWSCognitoIdentityService.GetId", RequestBody, GenericSuccess, GenericFailure);
}

void UAuthenticator::CognitoIdentityGetCredentialsForIdentity(const FString& IdentityID, const FString& IDToken, const FString& Issuer)
{
	const FString URL = this->BuildAWSURL("cognito-identity",this->WaasCredentials.GetIDPRegion());
	//FString RequestBody = FString::Printf(TEXT("{\"IdentityId\":\"%s\",\"Logins\":{\"%s\":\"%s\"}}"), *IdentityID, *Issuer, *IDToken);
	const FString RequestBody = "{\"IdentityId\":\""+ IdentityID +"\",\"Logins\":{\""+ Issuer +"\":\""+ IDToken +"\"}}";
							
	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
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
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
		this->CallAuthFailure();
	};

	this->RPC(URL,"AWSCognitoIdentityService.GetCredentialsForIdentity",RequestBody,GenericSuccess,GenericFailure);
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
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());

	const FString Verb = "POST";
	const FString CanonicalURI = "/";//we have none
	const FString CanQueryString = "";//no query parameters so leave this blank
	const FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();
	const FString CanHeaders = "content-type:application/x-amz-json-1.1\nhost:" + URI + "\nx-amz-content-sha256:" + HashPayload + "\nx-amz-date:" + BuildFullDateTime(Date) + "\nx-amz-target:TrentService.GenerateDataKey\n";
	const FString SignedHeaders = "content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target";

	//build the CanonicalRequest
	FString CanReq = Verb + "\n" + CanonicalURI + "\n" + CanQueryString + "\n" + CanHeaders + "\n" + SignedHeaders + "\n" + HashPayload;

	//cleanup
	delete[] PayloadBytes.Arr;
	return CanReq;
}

/*
* 
*/
FString UAuthenticator::BuildStringToSign(const FDateTime& Date, const FString& CanonicalRequest)
{
	const FUnsizedData PayloadBytes = StringToUTF8(CanonicalRequest);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());

	const FString CanonicalHash = BytesToHex(PayloadHashBytes.value,PayloadHashBytes.HASH_LEN);
	const FString FullDate = BuildFullDateTime(Date);
	FString StringToSign = "AWS4-HMAC-SHA256\n" + FullDate + "\n" + this->BuildScope(Date) + "\n" + CanonicalHash.ToLower();

	const FUnsizedData StringToSignBytes = StringToUTF8(StringToSign);
	const Sha256Hash StringToSignHashBytes = Sha256::getHash(StringToSignBytes.Arr, StringToSignBytes.GetLength());
	FString StringToSignHash = BytesToHex(StringToSignHashBytes.value, StringToSignHashBytes.HASH_LEN).ToLower();

	//cleanup
	delete[] PayloadBytes.Arr;
	delete[] StringToSignBytes.Arr;

	return StringToSign;
}

TArray<uint8_t> UAuthenticator::BuildSigningKey(const FDateTime& Date)
{
	const FUnsizedData DateKeyEncryptBytes = StringToUTF8("AWS4" + this->SecretKey);
	const FUnsizedData TimeStampBytes = StringToUTF8(BuildYYYYMMDD(Date));
	const FUnsizedData RegionBytes = StringToUTF8(this->WaasCredentials.GetKMSRegion());
	const FUnsizedData KMSBytes = StringToUTF8("kms");
	const FUnsizedData AWSReqBytes = StringToUTF8("aws4_request");

	const Sha256Hash DateKeyHash = Sha256::getHmac(DateKeyEncryptBytes.Arr,DateKeyEncryptBytes.GetLength(),TimeStampBytes.Arr,TimeStampBytes.GetLength());
	const Sha256Hash DateRegionKeyHash = Sha256::getHmac(DateKeyHash.value, DateKeyHash.HASH_LEN, RegionBytes.Arr, RegionBytes.GetLength());
	const Sha256Hash DateRegionServiceKeyHash = Sha256::getHmac(DateRegionKeyHash.value, DateRegionKeyHash.HASH_LEN, KMSBytes.Arr, KMSBytes.GetLength());
	const Sha256Hash SigningKeyHash = Sha256::getHmac(DateRegionServiceKeyHash.value, DateRegionServiceKeyHash.HASH_LEN, AWSReqBytes.Arr, AWSReqBytes.GetLength());

	FString DateKey = BytesToHex(DateKeyHash.value, DateKeyHash.HASH_LEN).ToLower();
	FString DateRegionKey = BytesToHex(DateRegionKeyHash.value,DateRegionKeyHash.HASH_LEN).ToLower();
	FString DateRegionServiceKey = BytesToHex(DateRegionServiceKeyHash.value, DateRegionServiceKeyHash.HASH_LEN).ToLower();
	FString SigningKey = BytesToHex(SigningKeyHash.value, SigningKeyHash.HASH_LEN).ToLower();

	TArray<uint8_t> SigningBytes;
	SigningBytes.Append(SigningKeyHash.value,SigningKeyHash.HASH_LEN);

	//cleanup
	delete[] DateKeyEncryptBytes.Arr;
	delete[] TimeStampBytes.Arr;
	delete[] RegionBytes.Arr;
	delete[] KMSBytes.Arr;
	delete[] AWSReqBytes.Arr;

	return SigningBytes;
}

FString UAuthenticator::BuildSignature(const TArray<uint8_t>& SigningKey, const FString& StringToSign)
{
	const FUnsizedData StringToSignBytes = StringToUTF8(StringToSign);
	const Sha256Hash SignatureHash = Sha256::getHmac(SigningKey.GetData(),SigningKey.Num(), StringToSignBytes.Arr, StringToSignBytes.GetLength());
	FString Signature = BytesToHex(SignatureHash.value,SignatureHash.HASH_LEN).ToLower();

	//cleanup
	delete[] StringToSignBytes.Arr;

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

void UAuthenticator::KMSGenerateDataKey(const FString& AWSKMSKeyID)
{
	const FString URL = BuildAWSURL("kms",this->WaasCredentials.GetKMSRegion());
	//FString RequestBody = FString::Printf(TEXT("{\"KeyId\":\"%s\",\"KeySpec\":\"AES_256\"}"), *AWSKMSKeyID);
	const FString RequestBody = "{\"KeyId\":\""+ AWSKMSKeyID +"\",\"KeySpec\":\"AES_256\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		const TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString PlainTextPtr, CipherTextBlobPtr;
		if (responseObj->TryGetStringField("Plaintext", PlainTextPtr) && responseObj->TryGetStringField("CipherTextBlob", CipherTextBlobPtr))
		{//good state
			TArray<uint8> PlainTextBytes;
			FBase64::Decode(PlainTextPtr, PlainTextBytes);
			this->PlainText = BytesToHex(PlainTextBytes.GetData(),PlainTextBytes.Num()).ToLower();

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
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
		this->CallAuthFailure();
	};
	const FDateTime cachedDate = FDateTime::UtcNow();
	this->AuthorizedRPC(this->BuildKMSAuthorizationHeader(cachedDate,URL.RightChop(8), RequestBody),cachedDate, URL, "TrentService.GenerateDataKey", RequestBody, GenericSuccess, GenericFailure);
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

void UAuthenticator::CognitoIdentityInitiateAuth(const FString& Email, const FString& AWSCognitoClientID)
{
	const FString URL = BuildAWSURL("cognito-idp",this->WaasCredentials.GetIDPRegion());
	//FString RequestBody = FString::Printf(TEXT("{\"AuthFlow\":\"CUSTOM_AUTH\",\"AuthParameters\":{\"USERNAME\":\"%s\"},\"ClientId\":\"%s\"}"), *Email, *AWSCognitoClientID);
	const FString RequestBody = "{\"AuthFlow\":\"CUSTOM_AUTH\",\"AuthParameters\":{\"USERNAME\":\""+ Email +"\"},\"ClientId\":\""+ AWSCognitoClientID +"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
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
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
		this->CallAuthFailure();
	};
	
	if (this->CanRetryEmailLogin())
	{
		this->RPC(URL, "AWSCognitoIdentityProviderService.InitiateAuth", RequestBody, GenericSuccess, GenericFailure);
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
	const int32 len = 12;
	for (int i = 0; i < len; i++)
	{
		pw += this->PWCharList[FMath::RandRange(0,this->PWCharList.Num()-1)];
	}
	return pw;
}

void UAuthenticator::CognitoIdentitySignUp(const FString& Email, const FString& Password, const FString& AWSCognitoClientID)
{
	const FString URL = BuildAWSURL("cognito-idp",this->WaasCredentials.GetIDPRegion());
	//FString RequestBody = FString::Printf(TEXT("{\"ClientId\":\"%s\",\"Password\":\"%s\",\"UserAttributes\":[{\"Name\":\"email\",\"Value\":\"%s\"}],\"Username\":\"%s\"}"), *AWSCognitoClientID, *Password, *Email, *Email);
	const FString RequestBody = "{\"ClientId\":\""+ AWSCognitoClientID +"\",\"Password\":\""+ Password +"\",\"UserAttributes\":[{\"Name\":\"email\",\"Value\":\""+ Email +"\"}],\"Username\":\""+ Email +"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		this->CognitoIdentityInitiateAuth(this->Cached_Email,this->WaasCredentials.GetCognitoClientId());
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
		this->CallAuthFailure();
	};

	this->RPC(URL, "AWSCognitoIdentityProviderService.SignUp", RequestBody, GenericSuccess, GenericFailure);
}

void UAuthenticator::AdminRespondToAuthChallenge(const FString& Email, const FString& Answer, const FString& ChallengeSessionString, const FString& AWSCognitoClientID)
{
	const FString URL = BuildAWSURL("cognito-idp",this->WaasCredentials.GetIDPRegion());
	//FString RequestBody = FString::Printf(TEXT("{\"ChallengeName\":\"CUSTOM_CHALLENGE\",\"ClientId\":\"%s\",\"Session\":\"%s\",\"ChallengeResponses\":{\"USERNAME\":\"%s\",\"ANSWER\":\"%s\"}}"), *AWSCognitoClientID, *ChallengeSessionString, *Email, *Answer);
	const FString RequestBody = "{\"ChallengeName\":\"CUSTOM_CHALLENGE\",\"ClientId\":\""+ AWSCognitoClientID +"\",\"Session\":\""+ ChallengeSessionString +"\",\"ChallengeResponses\":{\"USERNAME\":\""+ Email +"\",\"ANSWER\":\""+ Answer +"\"}}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
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
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
		this->CallAuthFailure();
	};

	this->RPC(URL, "AWSCognitoIdentityProviderService.RespondToAuthChallenge", RequestBody, GenericSuccess, GenericFailure);
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

FStoredCredentials_BE UAuthenticator::GetStoredCredentials()
{
	FCredentials_BE CredData;
	FCredentials_BE* Credentials = &CredData;
	const bool IsValid = this->GetStoredCredentials(Credentials);
	return FStoredCredentials_BE(IsValid, *Credentials);
}

void UAuthenticator::AuthWithSequence(const FString& IDTokenIn, const TArray<uint8_t>& Key)
{
	//Generates a new Random Wallet
	this->SessionWallet = new FWallet();
	int64 UnixIssueTime = FDateTime::UtcNow().ToUnixTimestamp();
	int64 UnixExpireTime = UnixIssueTime + 30;
	FString UnixIssueString = FString::Printf(TEXT("%lld"), UnixIssueTime);
	FString UnixExpireString = FString::Printf(TEXT("%lld"), UnixExpireTime);

	FString CachedWalletAddress = FString::Printf(TEXT("0x%s"),*BytesToHex(this->SessionWallet->GetWalletAddress().Arr, this->SessionWallet->GetWalletAddress().GetLength()).ToLower());

	//FString Intent = FString::Printf(TEXT("{\\\"version\\\":\\\"%s\\\",\\\"packet\\\":{\\\"code\\\":\\\"openSession\\\",\\\"expires\\\":%s,\\\"issued\\\":%s,\\\"session\\\":\\\"%s\\\",\\\"proof\\\":{\\\"idToken\\\":\\\"%s\\\"}}}"),*this->WaasVersion, *UnixExpireString, *UnixIssueString, *CachedWalletAddress, *IDTokenIn);
	FString Intent = "{\\\"version\\\":\\\""+ this->WaasVersion +"\\\",\\\"packet\\\":{\\\"code\\\":\\\"openSession\\\",\\\"expires\\\":"+ UnixExpireString +",\\\"issued\\\":"+ UnixIssueString +",\\\"session\\\":\\\""+ CachedWalletAddress +"\\\",\\\"proof\\\":{\\\"idToken\\\":\\\""+ IDTokenIn +"\\\"}}}";
	
	//FString Payload = FString::Printf(TEXT("{\"projectId\":%s,\"idToken\":\"%s\",\"sessionAddress\":\"%s\",\"friendlyName\":\"FRIENDLY SESSION WALLET\",\"intentJSON\":\"%s\"}"),*this->ProjectID, *IDTokenIn, *CachedWalletAddress, *Intent);
	FString Payload = "{\"projectId\":"+ this->WaasCredentials.GetProjectID() +",\"idToken\":\""+ IDTokenIn +"\",\"sessionAddress\":\""+ CachedWalletAddress +"\",\"friendlyName\":\"FRIENDLY SESSION WALLET\",\"intentJSON\":\""+ Intent +"\"}";
	
	UE_LOG(LogTemp, Display, TEXT("Payload: %s"), *Payload);

	TArray<uint8_t> TPayload = PKCS7(Payload);
	AES_ctx ctx;
	struct AES_ctx* PtrCtx = &ctx;

	const int32 IVSize = 16;
	TArray<uint8_t> iv;

	for (int i = 0; i < IVSize; i++)
		iv.Add(RandomByte());

	AES_init_ctx_iv(PtrCtx, Key.GetData(), iv.GetData());
	AES_CBC_encrypt_buffer(PtrCtx, TPayload.GetData(), TPayload.Num());

	FString PrePendedKey = BytesToHex(Key.GetData(),Key.Num());
	FString PrePendedIV = BytesToHex(iv.GetData(),iv.Num());
	FString PrePendedCipher = BytesToHex(TPayload.GetData(), TPayload.Num());

	FString PublicKey = BytesToHex(this->SessionWallet->GetWalletPublicKey().Arr,this->SessionWallet->GetWalletPublicKey().GetLength()).ToLower();
	FString PrivateKey = BytesToHex(this->SessionWallet->GetWalletPrivateKey().Arr,this->SessionWallet->GetWalletPrivateKey().GetLength()).ToLower();
	FString Address = BytesToHex(this->SessionWallet->GetWalletAddress().Arr,this->SessionWallet->GetWalletAddress().GetLength()).ToLower();

	//FString PayloadCipherText = FString::Printf(TEXT("0x%s%s"), *BytesToHex(iv.GetData(), iv.Num()).ToLower(), *BytesToHex(TPayload.GetData(), TPayload.Num()).ToLower());
	FString PayloadCipherText = "0x" + BytesToHex(iv.GetData(), iv.Num()).ToLower() + BytesToHex(TPayload.GetData(), TPayload.Num()).ToLower();
	
	TArray<uint8_t> PayloadSigBytes = this->SessionWallet->SignMessage(Payload);

	//FString PayloadSig = FString::Printf(TEXT("0x%s"),*BytesToHex(PayloadSigBytes.GetData(), PayloadSigBytes.Num()).ToLower());
	FString PayloadSig = "0x" + BytesToHex(PayloadSigBytes.GetData(), PayloadSigBytes.Num()).ToLower();
	
	//FString EncryptedPayloadKey = FString::Printf(TEXT("0x%s"),*this->CipherTextBlob);
	FString EncryptedPayloadKey = "0x" + this->CipherTextBlob;
	
	//FString FinalPayload = FString::Printf(TEXT("{\"payloadSig\":\"%s\",\"encryptedPayloadKey\":\"%s\",\"payloadCiphertext\":\"%s\"}"),*PayloadSig,*EncryptedPayloadKey,*PayloadCipherText);
	FString FinalPayload = "{\"payloadSig\":\""+ PayloadSig +"\",\"encryptedPayloadKey\":\""+ EncryptedPayloadKey +"\",\"payloadCiphertext\":\""+ PayloadCipherText +"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
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
				FString SessionPrivateKey = BytesToHex(this->SessionWallet->GetWalletPrivateKey().Arr, this->SessionWallet->GetWalletPrivateKey().GetLength()).ToLower();
				FCredentials_BE Credentials(this->PlainText, SessionPrivateKey, Id, Address, UserId, Subject, SessionId, Wallet, this->Cached_IDToken, this->Cached_Email, Issuer,IssuedDT.ToUnixTimestamp(), RefreshedDT.ToUnixTimestamp(), ExpiresDT.ToUnixTimestamp());
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
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
		this->CallAuthFailure();
	};

	this->SequenceRPC(this->WaasCredentials.GetRPCServer(), FinalPayload, GenericSuccess, GenericFailure);
}

void UAuthenticator::SequenceRPC(const FString& Url, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/json")
		->WithHeader("Accept", "application/json")
		->WithHeader("X-Access-Key", this->ProjectAccessKey)
		->WithVerb("POST")
		->WithContentAsString(RequestBody)
		->ProcessAndThen(OnSuccess, OnFailure);
}

void UAuthenticator::AuthorizedRPC(const FString& Authorization,const FDateTime& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	const FUnsizedData PayloadBytes = StringToUTF8(RequestBody);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());
	const FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();

	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithVerb("POST")
		->WithHeader("Host", Url.RightChop(8))
		->WithHeader("Content-type", "application/x-amz-json-1.1")
		->WithHeader("X-AMZ-TARGET", AMZTarget)
		->WithHeader("X-AMZ-DATE", BuildFullDateTime(Date))
		->WithHeader("X-AMZ-CONTENT-SHA256", HashPayload)
		->WithHeader("x-amz-security-token", this->SessionToken)
		->WithHeader("Authorization", Authorization)
		->WithContentAsString(RequestBody)
		->ProcessAndThen(OnSuccess, OnFailure);

	//cleanup
	delete[] PayloadBytes.Arr;
}

void UAuthenticator::RPC(const FString& Url, const FString& AMZTarget, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/x-amz-json-1.1")
		->WithHeader("X-AMZ-TARGET", AMZTarget)
		->WithVerb("POST")
		->WithContentAsString(RequestBody)
		->ProcessAndThen(OnSuccess, OnFailure);
}