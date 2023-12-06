// Fill out your copyright notice in the Description page of Project Settings.

#include "Authenticator.h"
#include "Misc/Guid.h"
#include "SequenceBackendManager.h"

//we always require a default constructor
UAuthenticator::UAuthenticator()
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
	this->Manager = nullptr;
}

void UAuthenticator::Init(ASequenceBackendManager * ManagerIn)
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
	this->Manager = ManagerIn;
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
	//CognitoIdentityGetID("","Google", IDTokenIn);//Outputs IdentityID
	//CognitoIdentityGetCredentialsForIdentity("IdentityID",IDTokenIn,"Google");//Outputs {accessKeyID, secretKey, sessionToken}
	//KMSGenerateDataKey();//outputs {plaintext, ciphertextblob, encryptedpayloadkey} the encryptedpayloadkey is the transport key
}

void UAuthenticator::EmailLogin(const FString& EmailIn)
{
	//CognitoIdentityInitiateAuth(TEXT("cc@cc.cc"), this->CognitoClientID);// returns a challengeSession //responds back with some poolid error (probably needs a proper cognito id to work
	//CognitoIdentitySignUp("cc@cc.cc", "password", this->CognitoClientID);
	//AdminRespondToAuthChallenge("cc@cc.cc", this->CognitoClientID, "AnswerCode", "challengeSession");
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
	FString RequestBody = "{\"IdentityPoolId\":\""+PoolID+"\",\"Logins\":{\""+Issuer+"\":\""+IDToken+"\"}}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"),*response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString IdentityIdPtr = TEXT("nil");
		if (responseObj->TryGetStringField("IdentityId",IdentityIdPtr))
		{//good state
			this->IdentityId = IdentityIdPtr;
		}
		else
		{//error state

		}
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"),*Error.Message);
	};

	this->RPC(URL,TEXT("com.amazonaws.cognito.identity.model.AWSCognitoIdentityService.GetId"), RequestBody, GenericSuccess, GenericFailure);
}

void UAuthenticator::CognitoIdentityGetCredentialsForIdentity(const FString& IdentityID, const FString& IDToken, const FString& Issuer)
{
	FString URL = this->BuildAWSURL("cognito-identity");
	FString RequestBody = "{\"IdentityID\":\""+IdentityID+"\",\"Logins\":{\""+Issuer+"\":\""+IDToken+"\"}}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString AccessKeyPtr, SecretKeyPtr, SessionTokenPtr;
		if (responseObj->TryGetStringField("AccessKeyId", AccessKeyPtr) && responseObj->TryGetStringField("SecretKey", SecretKeyPtr) && responseObj->TryGetStringField("SessionToken", SessionTokenPtr))
		{//good state
			this->AccessKeyId = AccessKeyPtr;
			this->SecretKey = SecretKeyPtr;
			this->SessionToken = SessionTokenPtr;
		}
		else
		{//error state

		}
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
	};

	this->RPC(URL,TEXT("com.amazonaws.cognito.identity.model.AWSCognitoIdentityService.GetCredentialsForIdentity"),RequestBody,GenericSuccess,GenericFailure);
}

void UAuthenticator::KMSGenerateDataKey()
{
	FString URL = BuildAWSURL("kms");
	//what is kmsKeyId? this isn't mentioned anywhere?
	FString RequestBody = "{\"KeyId\":\"kmsKeyId\",\"KeySpec\":\"AES_256\"}";

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

		}
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
	};

	this->RPC(URL,TEXT("TrentService.GenerateDataKey"), RequestBody,GenericSuccess,GenericFailure);
}

void UAuthenticator::CognitoIdentityInitiateAuth(const FString& Email, const FString& ClientID)
{
	FString URL = BuildAWSURL(TEXT("cognito-idp"));
	FString RequestBody = "{\"AuthFlow\":\"CUSTOM_AUTH\",\"AuthParameters\":{\"USERNAME\":\""+Email+"\"},\"ClientId\":\""+ClientID+"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString SessionPtr;
		if (responseObj->TryGetStringField("Session", SessionPtr))
		{//good state
			this->ChallengeSession = SessionPtr;
		}
		else
		{//error state

		}
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
	};
	
	this->RPC(URL,TEXT("AWSCognitoIdentityProviderService.InitiateAuth"), RequestBody,GenericSuccess,GenericFailure);
}

void UAuthenticator::CognitoIdentitySignUp(const FString& Email, const FString& Password, const FString& CognitoID)
{
	FString URL = BuildAWSURL("cognito-idp");
	FString RequestBody = "{\"ClientId\":\""+ CognitoID +"\",\"Password\":\""+ Password +"\",\"UserAttributes\":[{\"Name\":\""+ Email +"\",\"Value\":\""+ Email +"\"}],\"Username\":\""+ Email +"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		//don't need anything from this other than status code 200
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
	};

	this->RPC(URL, TEXT("AWSCognitoIdentityProviderService.SignUp"), RequestBody, GenericSuccess, GenericFailure);
}

void UAuthenticator::AdminRespondToAuthChallenge(const FString& Email, const FString& CognitoID, const FString& Answer, const FString& ChallengeSessionString)
{
	FString URL = BuildAWSURL("cognito-idp");
	FString RequestBody = "{\"ChallengeName\":\"CUSTOM_CHALLENGE\",\"ClientId\":\""+CognitoID+"\",\"Session\":\""+ChallengeSession+"\",\"ChallengeResponses\":{\"USERNAME\":\""+Email+"\",\"ANSWER\":\""+Answer+"\"}}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
		TSharedPtr<FJsonObject> responseObj = this->ResponseToJson(response);
		FString IDTokenPtr;
		if (responseObj->TryGetStringField("IdToken", IDTokenPtr))
		{//good state
			this->Cached_IDToken = IDTokenPtr;
		}
		else
		{//error state

		}
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
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