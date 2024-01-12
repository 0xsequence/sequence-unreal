// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RequestHandler.h"
#include "Util/Structs/BE_Enums.h"
#include "Types/Wallet.h"
#include "Authenticator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthRequiresCode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthFailure);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthSuccess);

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuthenticator : public UObject
{
	GENERATED_BODY()
//Delegates
public:
	FOnAuthRequiresCode AuthRequiresCode;
	FOnAuthFailure AuthFailure;
	FOnAuthSuccess AuthSuccess;

private://Broadcast handlers
	void CallAuthRequiresCode();
	void CallAuthFailure();
	void CallAuthSuccess();
//vars
private:
	TEnumAsByte<ESocialSigninType> SocialSigninType;

	FString StateToken = TEXT("");
	FString Nonce = TEXT("");

	FString UrlScheme = TEXT("powered-by-sequence");
	FString RedirectURL = TEXT("https://3d41-142-115-54-118.ngrok-free.app/");

	FString GoogleAuthURL = TEXT("https://accounts.google.com/o/oauth2/auth");
	FString GoogleClientID = TEXT("970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com");

	FString FacebookAuthURL = TEXT("https://www.facebook.com/v18.0/dialog/oauth");
	FString FacebookClientID = TEXT("");//TODO still need this

	FString DiscordAuthURL = TEXT("https://discord.com/api/oauth2/authorize");
	FString DiscordClientID = TEXT("");//TODO still need this

	FString AppleAuthURL = TEXT("https://appleid.apple.com/auth/authorize");
	FString AppleClientID = TEXT("");//TODO still need this

	FString Cached_IDToken;
	FString Cached_Email;
	//AWS
	FString IdentityPoolID = TEXT("us-east-2:42c9f39d-c935-4d5c-a845-5c8815c79ee3");
	FString UserPoolID = TEXT("FpPUBLAGt");
	FString Region = TEXT("us-east-2");
	FString AWSService = TEXT("kms");
	FString CognitoClientID = TEXT("5fl7dg7mvu534o9vfjbc6hj31p");
	FString KMSKeyID = TEXT("0fd8f803-9cb5-4de5-86e4-41963fb6043d");
	FString ProjectID = TEXT("124");
	FString ProjectAccessKey = TEXT("PXjUrkflseITe72Qsrz9adPBAAAAAAAAA");
	FString WaasVersion = TEXT("1.0.0");
	FString X_Sequence_Tenant = TEXT("9");
	FString WaasRPCURL = TEXT("https://d14tu8valot5m0.cloudfront.net/rpc");
	FString WaasAuthRPCURL = TEXT("https://d14tu8valot5m0.cloudfront.net/rpc/WaasAuthenticator/RegisterSession");

	// //us-east-2:170768627592
	//cognito-idp.us-east-1.amazonaws.com/us-east-1_123456789
	//Amazon Cognito user pool: cognito-idp.<region>.amazonaws.com/<YOUR_USER_POOL_ID>, for example, cognito-idp.us-east-1.amazonaws.com/us-east-1_123456789 
	//"cognito-idp.us-east-2.amazonaws.com/170768627592"
	const FString AWSIssuer = "cognito-idp."+this->Region+".amazonaws.com/"+this->Region+"_"+this->UserPoolID;
	TMap<ESocialSigninType, FString> ProviderMap = { {ESocialSigninType::Google,"accounts.google.com"},{ESocialSigninType::FaceBook,"graph.facebook.com"},{ESocialSigninType::AWS,AWSIssuer} };
	TArray<FString> PWCharList = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9"};

	const int32 EmailAuthMaxRetries = 2;
	int32 EmailAuthCurrRetries = EmailAuthMaxRetries;

	//From GetId
	FString IdentityId = TEXT("nil");

	//From GetCredentialsForIdentity
	FString AccessKeyId = TEXT("AKIAIOSFODNN7EXAMPLE");
	FString SecretKey = TEXT("wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY");
	FString SessionToken = TEXT("nil");

	//From KMSGenerateDataKey
	FString PlainText = TEXT("nil");
	FString CipherTextBlob = TEXT("nil");

	//From InitiateAuth
	FString ChallengeSession = TEXT("nil");

	//Gonna need to free this in a destructor unless I swap it to a UObject to use garbage collection
	FWallet * SessionWallet;
private:
	UAuthenticator();
public:
	FString GetSigninURL();

	FString GetRedirectURL();

	void SetSocialLoginType(ESocialSigninType Type);

	void SocialLogin(const FString& IDTokenIn);

	void EmailLogin(const FString& EmailIn);

	void EmailLoginCode(const FString& CodeIn);
private:
	bool CanRetryEmailLogin();

	void ResetRetryEmailLogin();

	FString GenerateSigninURL(FString AuthURL, FString ClientID);

	FString BuildAWSURL(const FString& Service);

	FString GenerateSignUpPassword();

	FString BuildYYYYMMDD(const FDateTime& Date);

	FString BuildFullDateTime(const FDateTime& Date);

	FString BuildScope(const FDateTime& Date);

	FString BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload);//step 1

	FString BuildStringToSign(const FDateTime& Date, const FString& CanonicalRequest);//step 2

	TArray<uint8_t> BuildSigningKey(const FDateTime& Date);//step 3

	FString BuildSignature(const TArray<uint8_t>& SigningKey, const FString& StringToSign);//step 4

	FString BuildKMSAuthorizationHeader(const FDateTime& Date, const FString& URI, const FString& Payload);

	//RPC Calls//

	void CognitoIdentityGetID(const FString& PoolID,const FString& Issuer, const FString& IDToken);

	void CognitoIdentityGetCredentialsForIdentity(const FString& IdentityID, const FString& IDToken, const FString& Issuer);

	void KMSGenerateDataKey();

	void CognitoIdentityInitiateAuth(const FString& Email);

	void CognitoIdentitySignUp(const FString& Email, const FString& Password);

	void AdminRespondToAuthChallenge(const FString& Email, const FString& Answer, const FString& ChallengeSessionString);

	//Sequence Specific//

public:	void AuthWithSequence(const FString& IDTokenIn, const TArray<uint8_t>& Key);

	//Sequence Specific//

	//RPC Calls//

	TSharedPtr<FJsonObject> ResponseToJson(const FString& response);

	void SequenceRPC(const FString& Url, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	void AuthorizedRPC(const FString& Authorization, const FDateTime& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	void RPC(const FString& Url,const FString& AMZTarget,const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
};