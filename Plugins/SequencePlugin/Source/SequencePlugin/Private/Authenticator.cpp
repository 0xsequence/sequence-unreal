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
	FString YYYYMMDD = "";

	YYYYMMDD.AppendInt(Date.GetYear());
	if (Date.GetMonth() < 10)
	{
		YYYYMMDD.AppendInt(0);
		YYYYMMDD.AppendInt(Date.GetMonth());
	}
	else
	{
		YYYYMMDD.AppendInt(Date.GetMonth());
	}

	if (Date.GetDay() < 10)
	{
		YYYYMMDD.AppendInt(0);
		YYYYMMDD.AppendInt(Date.GetDay());
	}
	else
	{
		YYYYMMDD.AppendInt(Date.GetDay());
	}

	return YYYYMMDD;
}

FString UAuthenticator::BuildScope(const FDateTime& Date)
{
	FString Scope = BuildYYYYMMDD(Date);
	Scope += "/" + this->Region + "/kms/aws4_request";
	return Scope;
}

FString UAuthenticator::BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload)
{
	FString CanQueryString = "";//no query parameters so leave this blank

	int32 PayloadSize = Payload.Len();
	uint8_t* PayloadBytePtr = new uint8_t[PayloadSize];
	PayloadSize = StringToBytes(Payload,PayloadBytePtr,PayloadSize);

	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytePtr, PayloadSize);
	delete[] PayloadBytePtr;
	FString HashPayload = BytesToHex(PayloadHashBytes.value, PayloadHashBytes.HASH_LEN);
	FString CanHeaders = "content-type:application/x-amz-json-1.1\nhost:" + URI.TrimStartAndEnd() + "\nx-amz-content-sha256:" + HashPayload.TrimStartAndEnd() + "\nx-amz-date:" + Date.ToIso8601().TrimStartAndEnd() + "\nx-amz-target:TrentService.GenerateDataKey";
	FString SignedHeaders = "content-type;host;x-amz-content-sha256;x-amz-date;x-amz-target";

	//build the CanonicalRequest
	FString CanReq = "POST\n";
	CanReq += URI + "\n" + CanQueryString + "\n" + CanHeaders + "\n" + SignedHeaders + "\n" + HashPayload;
	UE_LOG(LogTemp, Display, TEXT("Canonical Request:\n%s"), *CanReq);
	return CanReq;
}

FString UAuthenticator::BuildStringToSign(const FDateTime& Date, const FString& CanonicalRequest)
{
	int32 PayloadSize = CanonicalRequest.Len();
	uint8_t* PayloadBytePtr = new uint8_t[PayloadSize];
	PayloadSize = StringToBytes(CanonicalRequest, PayloadBytePtr, PayloadSize);

	const Sha256Hash PayloadHashBytes = Sha256::getHash(PayloadBytePtr, PayloadSize);
	delete[] PayloadBytePtr;

	FString CanonicalHash = BytesToHex(PayloadHashBytes.value,PayloadHashBytes.HASH_LEN);
	FString StringToSign = "AWS4-HMAC-SHA256\n";
	StringToSign += Date.ToIso8601() + "\n" + this->BuildScope(Date) + "\n" + CanonicalHash;
	UE_LOG(LogTemp, Display, TEXT("StringToSign:\n%s"), *StringToSign);
	return StringToSign;
}

FString UAuthenticator::BuildSigningKey(const FDateTime& Date)
{
	FString DateKeyEncryptKey = "AWS4" + this->SecretKey;
	FString TimeStamp = BuildYYYYMMDD(Date);

	FUnsizedData DateKeyEncryptBytes = StringToUTF8(DateKeyEncryptKey);
	FUnsizedData TimeStampBytes = StringToUTF8(TimeStamp);
	FUnsizedData RegionBytes = StringToUTF8(this->Region);
	FUnsizedData KMSBytes = StringToUTF8(this->KMSKeyID);
	FUnsizedData AWSReqBytes = StringToUTF8("aws4_request");

	Sha256Hash DateKeyHash = Sha256::getHmac(DateKeyEncryptBytes.Arr,DateKeyEncryptBytes.GetLength(),TimeStampBytes.Arr,TimeStampBytes.GetLength());
	Sha256Hash DateRegionKeyHash = Sha256::getHmac(DateKeyHash.value,DateKeyHash.HASH_LEN,RegionBytes.Arr,RegionBytes.GetLength());
	Sha256Hash DateRegionServiceKeyHash = Sha256::getHmac(DateRegionKeyHash.value,DateRegionKeyHash.HASH_LEN,KMSBytes.Arr,KMSBytes.GetLength());
	Sha256Hash SigningKeyHash = Sha256::getHmac(DateRegionServiceKeyHash.value,DateRegionServiceKeyHash.HASH_LEN,AWSReqBytes.Arr,AWSReqBytes.GetLength());
	FString SigningKey = BytesToHex(SigningKeyHash.value, SigningKeyHash.HASH_LEN);
	//DateKey = "hmac-sha256(AWS4+secret_access_key, yyyymmdd)";
	//DateRegionKey = "hmac-sha256(datekey,awsregion)";
	//DateRegionServiceKey = "hmac-sha256(dateregionkey,kms)";
	//SigningKey = "hmac-sha256(dateregionservicekey, aws4_request)";

	FString DateKey = BytesToHex(DateKeyHash.value, DateKeyHash.HASH_LEN);
	FString DateRegionKey = BytesToHex(DateRegionKeyHash.value,DateRegionKeyHash.HASH_LEN);
	FString DateRegionServiceKey = BytesToHex(DateRegionServiceKeyHash.value, DateRegionServiceKeyHash.HASH_LEN);

	UE_LOG(LogTemp, Display, TEXT("DateKey: %s"), *DateKey);
	UE_LOG(LogTemp, Display, TEXT("DateRegionKey: %s"), *DateRegionKey);
	UE_LOG(LogTemp, Display, TEXT("DateRegionServiceKey: %s"), *DateRegionServiceKey);
	UE_LOG(LogTemp, Display, TEXT("SigningKey: %s"), *SigningKey);

	return SigningKey;
}

FString UAuthenticator::BuildSignature(const FString& SigningKey, const FString& StringToSign)
{
	//Signature = "hex(hmac-sha256(signingkey,stringtosign))";
	int32 SigningKeySize = SigningKey.Len() / 2;//2 chars per byte
	uint8_t* SigningKeyBytes = new uint8_t[SigningKeySize];
	SigningKeySize = HexToBytes(SigningKey,SigningKeyBytes);

	int32 StringToSignSize = GetBytesInString(StringToSign);
	uint8_t* StringToSignPtr = new uint8_t[StringToSignSize];
	UE_LOG(LogTemp, Display, TEXT("Resulting Size: %d"), StringToSignSize);
	StringToSignSize = StringToBytes(StringToSign,StringToSignPtr,StringToSignSize);
	UE_LOG(LogTemp, Display, TEXT("Resulting Size: %d"), StringToSignSize);
	Sha256Hash SignatureHash = Sha256::getHmac(SigningKeyBytes,SigningKeySize,StringToSignPtr,StringToSignSize);
	FString Signature = BytesToHex(SignatureHash.value,SignatureHash.HASH_LEN);
	UE_LOG(LogTemp, Display, TEXT("Signature: %s"), *Signature);

	delete[] SigningKeyBytes;
	delete[] StringToSignPtr;

	return Signature;
}

FString UAuthenticator::BuildKMSAuthorizationHeader(const FDateTime& Date)
{
	FString tempId = "accountIdTemp";

	FString AuthHeader = "AWS4-HMAC-SHA256 Credential=";
	FString dateString = this->BuildYYYYMMDD(Date);

	AuthHeader += "AKIAIOSFODNN7EXAMPLE/"+ dateString +"/"+this->Region+"/kms/aws4_request,SignedHeaders=host;x-amz-date;x-amz-target,Signature=fe5f80f77d5fa3beca038a248ff027d0445342fe2855ddc963176630326f1024";

	/*
	* Authorization: AWS4-HMAC-SHA256 
	* Credential=AKIAIOSFODNN7EXAMPLE/20130524/us-east-1/s3/aws4_request, 
	* SignedHeaders=host;range;x-amz-date,
	* Signature=fe5f80f77d5fa3beca038a248ff027d0445342fe2855ddc963176630326f1024
	*/

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
	this->AuthorizedRPC(this->BuildKMSAuthorizationHeader(cachedDate), cachedDate.ToIso8601(), URL, TEXT("TrentService.GenerateDataKey"), RequestBody, GenericSuccess, GenericFailure);
}

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

void UAuthenticator::AuthorizedRPC(const FString& Authorization,const FString& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	NewObject<URequestHandler>()
		->PrepareRequest()
		->WithUrl(Url)
		->WithHeader("Content-type", "application/x-amz-json-1.1")
		->WithHeader("X-AMZ-TARGET", AMZTarget)
		->WithHeader("x-amz-date",Date)
		->WithHeader("Authorization",Authorization)
		->WithVerb("POST")
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