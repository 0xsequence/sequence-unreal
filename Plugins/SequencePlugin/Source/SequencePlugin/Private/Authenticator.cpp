// Fill out your copyright notice in the Description page of Project Settings.

#include "Authenticator.h"
#include "Misc/Guid.h"
#include "AES/aes.c"
#include "AES/aes.h"
#include "Eth/EthTransaction.h"
#include "Eth/Crypto.h"
#include "Types/Wallet.h"
#include "Bitcoin-Cryptography-Library/cpp/Keccak256.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256Hash.hpp"
#include "Bitcoin-Cryptography-Library/cpp/Sha256.hpp"
#include "Util/HexUtility.h"

//we always require a default constructor
UAuthenticator::UAuthenticator()
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
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

void UAuthenticator::CallAuthSuccess()
{
	if (this->AuthSuccess.IsBound())
		this->AuthSuccess.Broadcast();
	else
		UE_LOG(LogTemp, Error, TEXT("[System Error: nothing bound to delegate: AuthSuccess]"));
}

void UAuthenticator::SetSocialLoginType(ESocialSigninType Type)
{
	this->SocialSigninType = Type;
}

FString UAuthenticator::GetSigninURL()
{
	FString SigninURL = TEXT("");
	switch (this->SocialSigninType)
	{
	case ESocialSigninType::Google:
		SigninURL = this->GenerateSigninURL(GoogleAuthURL, GoogleClientID);
		break;
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
	CognitoIdentityGetID(this->IdentityPoolID, *ProviderMap.Find(this->SocialSigninType), IDTokenIn);
}

void UAuthenticator::EmailLogin(const FString& EmailIn)
{
	this->ResetRetryEmailLogin();
	this->Cached_Email = EmailIn;
	CognitoIdentityInitiateAuth(this->Cached_Email);
}

FString UAuthenticator::GenerateSigninURL(FString AuthURL, FString ClientID)
{
	//watch for trailing /'s in redirectURL
	FString ret = AuthURL + TEXT("?response_type=id_token&client_id=")+ClientID + TEXT("&redirect_uri=") + this->RedirectURL + TEXT("&scope=openid+profile+email&state=") + this->StateToken + TEXT("&nonce=") + this->Nonce;
	UE_LOG(LogTemp, Display, TEXT("Generated Signin URL: %s"), *ret);
	return ret;
}

FString UAuthenticator::BuildAWSURL(const FString& Service)
{
	return "https://" + Service + "." + this->Region+".amazonaws.com";
}

void UAuthenticator::CognitoIdentityGetID(const FString& PoolID,const FString& Issuer, const FString& IDToken)
{
	FString URL = this->BuildAWSURL(TEXT("cognito-identity"));
	FString RequestBody = "{\"IdentityPoolId\":\""+PoolID+"\",\"Logins\":{\""+Issuer+"\":\"" + IDToken + "\"}}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"),*response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString IdentityIdPtr = TEXT("nil");
		if (responseObj->TryGetStringField("IdentityId",IdentityIdPtr))
		{//good state
			this->IdentityId = IdentityIdPtr;
			CognitoIdentityGetCredentialsForIdentity(this->IdentityId, this->Cached_IDToken, *ProviderMap.Find(this->SocialSigninType));
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

	this->RPC(URL,TEXT("com.amazonaws.cognito.identity.model.AWSCognitoIdentityService.GetId"), RequestBody, GenericSuccess, GenericFailure);
}

void UAuthenticator::CognitoIdentityGetCredentialsForIdentity(const FString& IdentityID, const FString& IDToken, const FString& Issuer)
{
	FString URL = this->BuildAWSURL("cognito-identity");
	FString RequestBody = "{\"IdentityId\":\"" + IdentityID + "\",\"Logins\":{\""+Issuer+ "\":\"" + IDToken + "\"}}";
							
	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		const TSharedPtr<FJsonObject> *credObj;

		if (responseObj->TryGetObjectField("Credentials", credObj))
		{
			FString AccessKeyPtr, SecretKeyPtr, SessionTokenPtr;
			if (credObj->Get()->TryGetStringField("AccessKeyId", AccessKeyPtr) && credObj->Get()->TryGetStringField("SecretKey", SecretKeyPtr) && credObj->Get()->TryGetStringField("SessionToken", SessionTokenPtr))
			{//good state
				this->AccessKeyId = AccessKeyPtr;
				this->SecretKey = SecretKeyPtr;
				this->SessionToken = SessionTokenPtr;
				this->KMSGenerateDataKey();
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

	this->RPC(URL,TEXT("AWSCognitoIdentityService.GetCredentialsForIdentity"),RequestBody,GenericSuccess,GenericFailure);
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
	return BuildYYYYMMDD(Date) + "/" + this->Region + "/" + this->AWSService + "/aws4_request";
}

//In Testing
//FString UAuthenticator::BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload)
//{
//	FUnsizedData PayloadBytes = StringToUTF8(Payload);
//	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());
//
//	FString Verb = "POST";
//	FString CanonicalURI = "/";//we have none
//	FString CanQueryString = "";//no query parameters so leave this blank
//	FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();
//	FString CanHeaders = "content-type:application/x-amz-json-1.1\nhost:" + URI + "\nx-amz-content-sha256:" + HashPayload + "\nx-amz-date:" + BuildFullDateTime(Date) + "\nx-amz-target:TrentService.GenerateDataKey\n";
//	FString SignedHeaders = "content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target";
//
//	//build the CanonicalRequest
//	FString CanReq = Verb + "\n" + CanonicalURI + "\n" + CanQueryString + "\n" + CanHeaders + "\n" + SignedHeaders + "\n" + HashPayload;
//	UE_LOG(LogTemp, Display, TEXT("Canonical Request:\n%s"), *CanReq);
//	return CanReq;
//}

FString UAuthenticator::BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload)
{
	FUnsizedData PayloadBytes = StringToUTF8(Payload);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());

	FString Verb = "POST";
	FString CanonicalURI = "/";//we have none
	FString CanQueryString = "";//no query parameters so leave this blank
	FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();
	FString CanHeaders = "content-type:application/x-amz-json-1.1\nhost:" + URI + "\nx-amz-content-sha256:" + HashPayload + "\nx-amz-date:" + BuildFullDateTime(Date) + "\nx-amz-target:TrentService.GenerateDataKey\n";
	FString SignedHeaders = "content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target";

	//build the CanonicalRequest
	FString CanReq = Verb + "\n" + CanonicalURI + "\n" + CanQueryString + "\n" + CanHeaders + "\n" + SignedHeaders + "\n" + HashPayload;
	UE_LOG(LogTemp, Display, TEXT("Canonical Request:\n%s"), *CanReq);
	return CanReq;
}

//Good Copy (Works!)
//FString UAuthenticator::BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload)
//{
//	FUnsizedData PayloadBytes = StringToUTF8(Payload);
//	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());
//
//	FString Host = URI;
//	FString Verb = "GET";
//	FString CanonicalURI = "/test.txt";
//	FString CanQueryString = "";//no query parameters so leave this blank
//	FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();
//	FString CanHeaders = "host:" + Host + "\nrange:bytes=0-9\nx-amz-content-sha256:" + HashPayload + "\nx-amz-date:" + BuildFullDateTime(Date) + "\n";
//	FString SignedHeaders = "host;range;x-amz-content-sha256;x-amz-date";
//
//	//build the CanonicalRequest
//	FString CanReq = Verb + "\n" + CanonicalURI + "\n" + CanQueryString + "\n" + CanHeaders + "\n" + SignedHeaders + "\n" + HashPayload;
//	UE_LOG(LogTemp, Display, TEXT("Canonical Request:\n%s"), *CanReq);
//	return CanReq;
//}

/*
* DateKey = hmac( AWS4wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY , 20130524 ) = 68896419206d6240ad4cd7dc8ba658efbf3b43b53041950083a10833824fcfbb
* DateRegionKey = hmac( DateKey , us-east-1 ) = b1d69b01d01fbfab62ce62e2b354dc81fa797232685c3de02919930c87f3db5d
* DateRegionServiceKey hmac( DateRegionKey , s3 ) = ec603b02e46102b2c2563dd47216472c5c0aba27edeb8308255e4c60bb07bda0
* SigningKey hmac( DateRegionServiceKey , aws4_request) = d949da6fe2897897d73557446db35c06dc34feb7f74e7d949c6fe9d674a02103
* Signature: f03131e53fcdcd3605054f5ead58370d14a672add94bda5da0a69d65d03e7edc
*/
FString UAuthenticator::BuildStringToSign(const FDateTime& Date, const FString& CanonicalRequest)
{
	FUnsizedData PayloadBytes = StringToUTF8(CanonicalRequest);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());

	FString CanonicalHash = BytesToHex(PayloadHashBytes.value,PayloadHashBytes.HASH_LEN);
	FString FullDate = BuildFullDateTime(Date);
	FString StringToSign = "AWS4-HMAC-SHA256\n"+FullDate+"\n"+this->BuildScope(Date)+"\n"+CanonicalHash.ToLower();
	UE_LOG(LogTemp, Display, TEXT("StringToSign:\n%s"), *StringToSign);

	FUnsizedData StringToSignBytes = StringToUTF8(StringToSign);
	const Sha256Hash StringToSignHashBytes = Sha256::getHash(StringToSignBytes.Arr, StringToSignBytes.GetLength());
	FString StringToSignHash = BytesToHex(StringToSignHashBytes.value, StringToSignHashBytes.HASH_LEN).ToLower();
	UE_LOG(LogTemp, Display, TEXT("StringToSignHash:\n%s"), *StringToSignHash);

	return StringToSign;
}

TArray<uint8_t> UAuthenticator::BuildSigningKey(const FDateTime& Date)
{
	FUnsizedData DateKeyEncryptBytes = StringToUTF8("AWS4" + this->SecretKey);
	FUnsizedData TimeStampBytes = StringToUTF8(BuildYYYYMMDD(Date));
	FUnsizedData RegionBytes = StringToUTF8(this->Region);
	FUnsizedData KMSBytes = StringToUTF8(this->AWSService);
	FUnsizedData AWSReqBytes = StringToUTF8("aws4_request");

	Sha256Hash DateKeyHash = Sha256::getHmac(DateKeyEncryptBytes.Arr,DateKeyEncryptBytes.GetLength(),TimeStampBytes.Arr,TimeStampBytes.GetLength());
	Sha256Hash DateRegionKeyHash = Sha256::getHmac(DateKeyHash.value, DateKeyHash.HASH_LEN, RegionBytes.Arr, RegionBytes.GetLength());
	Sha256Hash DateRegionServiceKeyHash = Sha256::getHmac(DateRegionKeyHash.value, DateRegionKeyHash.HASH_LEN, KMSBytes.Arr, KMSBytes.GetLength());
	Sha256Hash SigningKeyHash = Sha256::getHmac(DateRegionServiceKeyHash.value, DateRegionServiceKeyHash.HASH_LEN, AWSReqBytes.Arr, AWSReqBytes.GetLength());

	FString DateKey = BytesToHex(DateKeyHash.value, DateKeyHash.HASH_LEN).ToLower();
	FString DateRegionKey = BytesToHex(DateRegionKeyHash.value,DateRegionKeyHash.HASH_LEN).ToLower();
	FString DateRegionServiceKey = BytesToHex(DateRegionServiceKeyHash.value, DateRegionServiceKeyHash.HASH_LEN).ToLower();
	FString SigningKey = BytesToHex(SigningKeyHash.value, SigningKeyHash.HASH_LEN).ToLower();

	TArray<uint8_t> SigningBytes;
	SigningBytes.Append(SigningKeyHash.value,SigningKeyHash.HASH_LEN);

	UE_LOG(LogTemp, Display, TEXT("InitialKey: %s"), *("AWS4" + this->SecretKey));
	UE_LOG(LogTemp, Display, TEXT("Date: %s"), *BuildYYYYMMDD(Date));
	UE_LOG(LogTemp, Display, TEXT("DateKey: %s"), *DateKey);
	UE_LOG(LogTemp, Display, TEXT("DateRegionKey: %s"), *DateRegionKey);
	UE_LOG(LogTemp, Display, TEXT("DateRegionServiceKey: %s"), *DateRegionServiceKey);
	UE_LOG(LogTemp, Display, TEXT("SigningKey: %s"), *SigningKey);

	return SigningBytes;
}

FString UAuthenticator::BuildSignature(const TArray<uint8_t>& SigningKey, const FString& StringToSign)
{
	//Signature = "hex(hmac-sha256(signingkey,stringtosign))";
	FUnsizedData StringToSignBytes = StringToUTF8(StringToSign);
	Sha256Hash SignatureHash = Sha256::getHmac(SigningKey.GetData(),SigningKey.Num(), StringToSignBytes.Arr, StringToSignBytes.GetLength());
	FString Signature = BytesToHex(SignatureHash.value,SignatureHash.HASH_LEN).ToLower();
	UE_LOG(LogTemp, Display, TEXT("Signature: %s"), *Signature);
	return Signature;
}

FString UAuthenticator::BuildKMSAuthorizationHeader(const FDateTime& Date, const FString& URI, const FString& Payload)
{
	FString AuthHeader = "AWS4-HMAC-SHA256 Credential=";
	FString dateString = this->BuildYYYYMMDD(Date);
	FString CanReq = this->BuildCanonicalRequest(URI, Date, Payload);
	FString StringToSign = this->BuildStringToSign(Date, CanReq);
	TArray<uint8_t> SigningKey = this->BuildSigningKey(Date);
	FString Signature = this->BuildSignature(SigningKey, StringToSign);
	AuthHeader += this->AccessKeyId + "/" + dateString + "/" + this->Region + "/" + this->AWSService + "/aws4_request,SignedHeaders=content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target,Signature=" + Signature;

	UE_LOG(LogTemp, Display, TEXT("AccessKey: %s"), *this->AccessKeyId);
	UE_LOG(LogTemp, Display, TEXT("SecretKey: %s"), *this->SecretKey);
	UE_LOG(LogTemp, Display, TEXT("SessionToken: %s"), *this->SessionToken);


	return AuthHeader;
}

void UAuthenticator::KMSGenerateDataKey()
{
	FString URL = BuildAWSURL("kms");

	FString RequestBody = "{\"KeyId\":\""+this->KMSKeyID+"\",\"KeySpec\":\"AES_256\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString PlainTextPtr, CipherTextBlobPtr;
		if (responseObj->TryGetStringField("Plaintext", PlainTextPtr) && responseObj->TryGetStringField("CipherTextBlob", CipherTextBlobPtr))
		{//good state
			this->PlainText = PlainTextPtr;
			this->CipherTextBlob = CipherTextBlobPtr;
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
	FDateTime cachedDate = FDateTime::UtcNow();
	this->AuthorizedRPC(this->BuildKMSAuthorizationHeader(cachedDate,URL.RightChop(8), RequestBody),cachedDate, URL, TEXT("TrentService.GenerateDataKey"), RequestBody, GenericSuccess, GenericFailure);
}

//curl https://kms.us-east-2.amazonaws.com -X POST -H "x-amz-target: TrentService.GenerateDataKey" -H "Host: kms.us-east-2.amazonaws.com" -H "Content-Type: application/x-www-form-urlencoded; charset=utf-8" -H "X-Amz-Date: 20240111T162901Z" -H "Authorization: AWS4-HMAC-SHA256 Credential=ASIASPQU2O6EONAHVAND/20240111/us-east-2/kms/aws4_request, SignedHeaders=content-length;content-type;host;x-amz-date, Signature=2d55c1460bc46dc42a8d716496ddd8f83ef647ac5fa459d9a57254dedeec1076" --data-binary '{"KeyId":"0fd8f803-9cb5-4de5-86e4-41963fb6043d","KeySpec":"AES_256"}'

bool UAuthenticator::CanRetryEmailLogin()
{
	bool CanRetry = this->EmailAuthCurrRetries > 0;
	this->EmailAuthCurrRetries--;
	return CanRetry;
}

void UAuthenticator::ResetRetryEmailLogin()
{
	this->EmailAuthCurrRetries = this->EmailAuthMaxRetries;
}

void UAuthenticator::CognitoIdentityInitiateAuth(const FString& Email)
{
	FString URL = BuildAWSURL(TEXT("cognito-idp"));
	FString RequestBody = "{\"AuthFlow\":\"CUSTOM_AUTH\",\"AuthParameters\":{\"USERNAME\":\""+Email+"\"},\"ClientId\":\""+this->CognitoClientID+"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
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
				this->CognitoIdentitySignUp(this->Cached_Email, this->GenerateSignUpPassword());
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
		this->RPC(URL, TEXT("AWSCognitoIdentityProviderService.InitiateAuth"), RequestBody, GenericSuccess, GenericFailure);
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

void UAuthenticator::CognitoIdentitySignUp(const FString& Email, const FString& Password)
{
	FString URL = BuildAWSURL("cognito-idp");
	FString RequestBody = "{\"ClientId\":\""+ this->CognitoClientID +"\",\"Password\":\""+ Password +"\",\"UserAttributes\":[{\"Name\":\"email\",\"Value\":\""+ Email +"\"}],\"Username\":\""+ Email +"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		this->CognitoIdentityInitiateAuth(this->Cached_Email);
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
		this->CallAuthFailure();
	};

	this->RPC(URL, TEXT("AWSCognitoIdentityProviderService.SignUp"), RequestBody, GenericSuccess, GenericFailure);
}

void UAuthenticator::AdminRespondToAuthChallenge(const FString& Email, const FString& Answer, const FString& ChallengeSessionString)
{
	FString URL = BuildAWSURL("cognito-idp");
	FString RequestBody = "{\"ChallengeName\":\"CUSTOM_CHALLENGE\",\"ClientId\":\""+this->CognitoClientID+"\",\"Session\":\""+ChallengeSession+"\",\"ChallengeResponses\":{\"USERNAME\":\""+Email+"\",\"ANSWER\":\""+Answer+"\"}}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString IDTokenPtr;
		const TSharedPtr<FJsonObject> *AuthObject;
		if (responseObj->TryGetObjectField("AuthenticationResult", AuthObject))
		{
			if (AuthObject->Get()->TryGetStringField("IdToken", IDTokenPtr))
			{//good state
				this->Cached_IDToken = IDTokenPtr;
				this->SetSocialLoginType(ESocialSigninType::AWS);
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

	this->RPC(URL, TEXT("AWSCognitoIdentityProviderService.RespondToAuthChallenge"), RequestBody, GenericSuccess, GenericFailure);
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
	this->AdminRespondToAuthChallenge(this->Cached_Email,CodeIn,this->ChallengeSession);
}

void UAuthenticator::AuthWithSequence(const FString& IDTokenIn, const TArray<uint8_t>& Key)
{
	//Generates a new Random Wallet
	this->SessionWallet = new FWallet();
	FString CachedWalletAddress = BytesToHex(this->SessionWallet->GetWalletAddress().Arr, this->SessionWallet->GetWalletAddress().GetLength());
	FString Intent = "{\"version\":\"1.0.0\",\"packet\":{\"code\":\"openSession\",\"session\":\""+ CachedWalletAddress + "\",\"proof\":{\"idToken\":\"" + IDTokenIn + "\"}}}";
	FString Payload = "{\"projectId\":\"projectID_IN\",\"idToken\":\""+IDTokenIn+"\",\"sessionAddress\":\""+ CachedWalletAddress +"\",\"friendlyName\":\"FRIENDLY SESSION WALLET\",\"intentJSON\":\""+Intent+"\"}";

	TArray<uint8_t> TPayload = PKCS7(Payload);

	//need to encrypt the payload string now
	AES_ctx ctx;
	struct AES_ctx * PtrCtx = &ctx;

	const int32 IVSize = 16;
	TArray<uint8_t> iv;

	for (int i = 0; i < IVSize; i++)
	{
		iv.Add(RandomByte());
	}

	AES_init_ctx_iv(PtrCtx, Key.GetData(), iv.GetData());
	AES_CBC_encrypt_buffer(PtrCtx, TPayload.GetData(), TPayload.Num());

	TPayload.Append(iv);//append the IV onto the encrypted payload
	FString EncryptedPayload = BytesToHex(TPayload.GetData(),TPayload.Num());//cipher text

	FHash256 SuffixHash = FHash256::New();
	FUnsizedData EncryptedSigningData = StringToUTF8(EncryptedPayload);
	Keccak256::getHash(EncryptedSigningData.Arr, EncryptedSigningData.GetLength(), SuffixHash.Arr);
	
	//this below may not actually be needed?

	//FString prefix = "\x19";
	//FString mid = "Ethereum Signed Message:\n32";
	//FString suffix = BytesToHex(SuffixHash.Arr, SuffixHash.GetLength());

	//FString testString = prefix + mid + suffix;

	//FHash256 MesgHash = FHash256::New();
	//FUnsizedData MesgData = StringToUTF8(prefix + mid + suffix);
	//Keccak256::getHash(MesgData.Arr,MesgData.GetLength(),MesgHash.Arr);

	//FString MesgStr = BytesToHex(MesgHash.Arr,MesgHash.GetLength());

	//TArray<uint8> sig = this->SessionWallet->SignMessage(MesgStr);
	//FString sigStr = BytesToHex(sig.GetData(),sig.Num());

	////for signature verification incase something is going wrong we can verify this data using ext. tools
	//UE_LOG(LogTemp, Display, TEXT("Adr: %s"), *CachedWalletAddress);
	//UE_LOG(LogTemp, Display, TEXT("Msg: %s"), *MesgStr);
	//UE_LOG(LogTemp, Display, TEXT("Sig: %s"), *sigStr);
}

void UAuthenticator::AuthorizedRPC(const FString& Authorization,const FDateTime& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	FUnsizedData PayloadBytes = StringToUTF8(RequestBody);
	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytes.Arr, PayloadBytes.GetLength());
	FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN).ToLower();

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
		->WithHeader("Authorization",Authorization)
		->WithContentAsString(RequestBody)
		->ProcessAndThen(OnSuccess, OnFailure);
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