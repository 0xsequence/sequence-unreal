// Fill out your copyright notice in the Description page of Project Settings.

#include "Authenticator.h"
#include "Misc/Guid.h"

UAuthenticator::UAuthenticator()
{
	this->Nonce = FGuid::NewGuid().ToString();
	this->StateToken = FGuid::NewGuid().ToString();
	this->AppID = FGuid::NewGuid().ToString();
}

FString UAuthenticator::GetSigninURL()
{
	return this->GenerateSigninURL(GoogleAuthURL, GoogleClientID);
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
	CognitoIdentityInitiateAuth(TEXT("cc@cc.cc"), this->AppID);
}

void UAuthenticator::EmailLogin(const FString& EmailIn)
{

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
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
	};

	this->RPC(URL,TEXT("com.amazonaws.cognito.identity.model.AWSCognitoIdentityService.GetCredentialsForIdentity"),RequestBody,GenericSuccess,GenericFailure);
}

void UAuthenticator::KMSGenerateDataKey()
{
	FString URL = BuildAWSURL("kms");//what is kmsKeyId? this isn't mentioned anywhere?
	FString RequestBody = "{\"KeyId\":\"kmsKeyId\",\"KeySpec\":\"AES_256\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
	};

	this->RPC(URL,TEXT("TrentService.GenerateDataKey"),RequestBody,GenericSuccess,GenericFailure);
}

void UAuthenticator::CognitoIdentityInitiateAuth(const FString& Email, const FString& ClientID)
{
	FString URL = BuildAWSURL(TEXT("cognito-idp"));
	FString ResponseBody = "{\"AuthFlow\":\"CUSTOM_AUTH\",\"AuthParameters\":{\"USERNAME\":\""+Email+"\"},\"ClientId\":\""+ClientID+"\"}";

	const TSuccessCallback<FString> GenericSuccess = [this](const FString response)
	{
		UE_LOG(LogTemp, Display, TEXT("Response %s"), *response);
	};

	const FFailureCallback GenericFailure = [this](const FSequenceError Error)
	{
		UE_LOG(LogTemp, Display, TEXT("[Error: %s]"), *Error.Message);
	};
	
	this->RPC(URL,TEXT("AWSCognitoIdentityProviderService.InitiateAuth"),ResponseBody,GenericSuccess,GenericFailure);
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