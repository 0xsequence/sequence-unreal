// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RequestHandler.h"
#include "Util/Structs/BE_Enums.h"
#include "Authenticator.generated.h"

class ASequenceBackendManager;

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuthenticator : public UObject
{
	GENERATED_BODY()
//vars
private:
	ASequenceBackendManager* Manager;
	ESocialSigninType SocialSigninType;

	const uint16 WINDOWS_IPC_PORT = 52836;
	FString StateToken = TEXT("");
	FString Nonce = TEXT("");

	FString UrlScheme = TEXT("powered-by-sequence");
	//need to know how to override this / get this out of browser
	FString RedirectURL = TEXT("https://3d41-142-115-54-118.ngrok-free.app/");

	FString GoogleAuthURL = TEXT("https://accounts.google.com/o/oauth2/auth");
	FString GoogleClientID = TEXT("970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com");

	FString FacebookAuthURL = TEXT("https://www.facebook.com/v18.0/dialog/oauth");
	FString FacebookClientID = TEXT("");//TODO still need this

	FString DiscordAuthURL = TEXT("https://discord.com/api/oauth2/authorize");
	FString DiscordClientID = TEXT("");//TODO still need this

	FString AppleAuthURL = TEXT("https://appleid.apple.com/auth/authorize");
	FString AppleClientID = TEXT("");//TODO still need this

	FString Cached_IDToken;//not sure if I need this or not permenately

	//AWS
	FString IdentityPoolID = TEXT("nil");//TODO still need this
	FString Region = TEXT("us-east-2");//TODO need to know how we are doing region handling for AWS
	FString CognitoClientID = TEXT("nil");//TODO still need this
	FString KMSKeyID = TEXT("nil");//TODO still need this
	FString ProjectID = TEXT("nil");//TODO still need this

	//From GetId
	FString IdentityId = TEXT("nil");

	//From GetCredentialsForIdentity
	FString AccessKeyId = TEXT("nil");
	FString SecretKey = TEXT("nil");
	FString SessionToken = TEXT("nil");

	//From KMSGenerateDataKey
	FString PlainText = TEXT("nil");
	FString CipherTextBlob = TEXT("nil");//this is the transport key and needs to be secured

	//From InitiateAuth
	FString ChallengeSession = TEXT("nil");

private:
	UAuthenticator();//hide this as we only want to be able to call the parameterized version
public:
	void Init(ASequenceBackendManager* ManagerIn);

	FString GetSigninURL();

	FString GetRedirectURL();

	void SetSocialLoginType(ESocialSigninType Type);

	void SocialLogin(const FString& IDTokenIn);

	void EmailLogin(const FString& EmailIn);

	void EmailLoginCode(const FString& CodeIn);

	void EmailCodeCallout();
private:
	FString GenerateSigninURL(FString AuthURL, FString ClientID);

	FString BuildAWSURL(const FString& Service);

	//RPC Calls//

	void CognitoIdentityGetID(const FString& PoolID,const FString& Issuer, const FString& IDToken);

	void CognitoIdentityGetCredentialsForIdentity(const FString& IdentityID, const FString& IDToken, const FString& Issuer);

	void KMSGenerateDataKey();

	void CognitoIdentityInitiateAuth(const FString& Email, const FString& ClientID);

	void CognitoIdentitySignUp(const FString& Email, const FString& Password, const FString& CognitoID);

	void AdminRespondToAuthChallenge(const FString& Email, const FString& CognitoID, const FString& Answer, const FString& ChallengeSessionString);

	//RPC Calls//

	TSharedPtr<FJsonObject> ResponseToJson(const FString& response);


	void RPC(const FString& Url,const FString& AMZTarget,const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
};
