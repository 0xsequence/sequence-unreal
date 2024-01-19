// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RequestHandler.h"
#include "Util/Structs/BE_Enums.h"
#include "Types/Wallet.h"
#include "Authenticator.generated.h"


struct FSSOCredentials
{
	FString URL = "";
	FString ClientID = "";
	FSSOCredentials(const FString& URLIn, const FString& ClientIDIn)
	{
		URL = URLIn;
		ClientID = ClientIDIn;
	}
};

USTRUCT(BlueprintType)
struct FCredentials_BE
{
    GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
    FString TransportKey = "";
	UPROPERTY()
    FString SessionPrivateKey = "";
	UPROPERTY()
    FString Id = "";
	UPROPERTY()
    FString Address = "";
	UPROPERTY()
    FString UserId = "";
	UPROPERTY()
    FString Subject = "";
	UPROPERTY()
    FString SessionId = "";
	UPROPERTY()
    FString WalletAddress = "";
	UPROPERTY()
    FString IDToken = "";
	UPROPERTY()
    FString Email = "";
	UPROPERTY()
    FString Issuer = "";
public:
	FCredentials_BE(){}

    FCredentials_BE(const FString& TransportKeyIn,const FString& SessionPrivateKeyIn, const FString& IdIn, const FString& AddressIn,const FString& UserIdIn, const FString& SubjectIn, const FString& SessionIdIn, const FString& WalletAddressIn,const FString& IDTokenIn, const FString& EmailIn, const FString& IssuerIn)
    {
		TransportKey = TransportKeyIn;
		SessionPrivateKey = SessionPrivateKeyIn;
		Id = IdIn;
		Address = AddressIn;
		UserId = UserIdIn;
		Subject = SubjectIn;
		SessionId = SessionIdIn;
		WalletAddress = WalletAddressIn;
		IDToken = IDTokenIn;
		Email = EmailIn;
		Issuer = IssuerIn;
    }

	FString GetTransportKey()
	{
		return TransportKey;
	}

	FString GetSessionPrivateKey()
	{
		return SessionPrivateKey;
	}

	FString GetSessionPublicKey()
	{
		FWallet TWallet = FWallet(SessionPrivateKey);
		FString PublicKeyStr = BytesToHex(TWallet.GetWalletPublicKey().Arr,TWallet.GetWalletPublicKey().GetLength()).ToLower();
		TWallet.~FWallet();
		return PublicKeyStr;
	}

	FString GetSessionAddress()
	{
		FWallet TWallet = FWallet(SessionPrivateKey);
		FString AddressStr = BytesToHex(TWallet.GetWalletAddress().Arr, TWallet.GetWalletAddress().GetLength()).ToLower();
		TWallet.~FWallet();
		return AddressStr;
	}

	FString SignMessageWithSessionWallet(const FString& Message)
	{
		FWallet TWallet = FWallet(SessionPrivateKey);
		TArray<uint8> SigBytes = TWallet.SignMessage(Message);
		FString Signature = BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
		TWallet.~FWallet();
		return Signature;
	}

	FString GetId()
	{
		return Id;
	}

	FString GetAddress()
	{
		return Address;
	}

	FString GetUserId()
	{
		return UserId;
	}

	FString GetSubject()
	{
		return Subject;
	}

	FString GetSessionId()
	{
		return SessionId;
	}

	FString GetWalletAddress()
	{
		return WalletAddress;
	}

	FString GetIDToken()
	{
		return IDToken;
	}

	FString GetEmail()
	{
		return Email;
	}

	FString GetIssuer()
	{
		return Issuer;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthRequiresCode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAuthFailure);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuthSuccess, FCredentials_BE, Credentials);

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
	void CallAuthSuccess(const FCredentials_BE& Credentials);
//vars
private:
	const FString SaveSlot = "Cr";
	const uint32 UserIndex = 0;

	FString StateToken = "";
	FString Nonce = "";

	const FString UrlScheme = "powered-by-sequence";
	const FString RedirectURL = "https://3d41-142-115-54-118.ngrok-free.app/";

	const FString GoogleAuthURL = "https://accounts.google.com/o/oauth2/auth";
	const FString GoogleClientID = "970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com";

	const FString FacebookAuthURL = "https://www.facebook.com/v18.0/dialog/oauth";
	const FString FacebookClientID = "";//TODO still need this

	const FString DiscordAuthURL = "https://discord.com/api/oauth2/authorize";
	const FString DiscordClientID = "";//TODO still need this

	const FString AppleAuthURL = "https://appleid.apple.com/auth/authorize";
	const FString AppleClientID = "";//TODO still need this

	FString Cached_IDToken;
	FString Cached_Email;
	//AWS

	const FString IdentityPoolID = "us-east-2:9747b3b1-c831-4efd-8aee-ac362373ad53";
	const FString Region = "us-east-2";
	const FString AWSService = "kms";
	const FString CognitoClientID = "3fd4tq7gvroie1romfslk2nvv8";
	const FString KMSKeyID = "0fd8f803-9cb5-4de5-86e4-41963fb6043d";
	const FString ProjectID = "124";
	const FString ProjectAccessKey = "AAAAAAAAAAAfAAAAAAAAAA";
	const FString WaasVersion = "1.0.0";
	const FString WaasAuthRPCURL = "https://d3jwb7a1rcpkmp.cloudfront.net/rpc/WaasAuthenticator/RegisterSession";

	const TMap<ESocialSigninType, FSSOCredentials> SSOProviderMap = { {ESocialSigninType::Google,FSSOCredentials(GoogleAuthURL,GoogleClientID)}};
	TArray<FString> PWCharList = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9"};

	const int32 EmailAuthMaxRetries = 2;
	int32 EmailAuthCurrRetries = EmailAuthMaxRetries;

	//From GetId
	FString IdentityId = "";

	//From GetCredentialsForIdentity
	FString AccessKeyId = "AKIAIOSFODNN7EXAMPLE";
	FString SecretKey = "wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY";
	FString SessionToken = "";

	//From KMSGenerateDataKey
	FString PlainText = "";
	FString CipherTextBlob = "";

	//From InitiateAuth
	FString ChallengeSession = "";

	//Gonna need to free this in a destructor unless I swap it to a UObject to use garbage collection
	FWallet * SessionWallet;
private:
	UAuthenticator();
public:
	FString GetSigninURL(const ESocialSigninType& Type);

	FString GetRedirectURL();

	void SocialLogin(const FString& IDTokenIn);

	void EmailLogin(const FString& EmailIn);

	void EmailLoginCode(const FString& CodeIn);
private:
	void StoreCredentials(const FCredentials_BE& Credentials);
	bool GetStoredCredentials(FCredentials_BE * Credentials);

	FString GetISSClaim(const FString& JWT);

	bool CanRetryEmailLogin();

	void ResetRetryEmailLogin();

	FString GenerateSigninURL(const FString& AuthURL, const FString& ClientID);

	FString BuildAWSURL(const FString& Service, const FString& AWSRegion);

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

	void KMSGenerateDataKey(const FString& AWSKMSKeyID);

	void CognitoIdentityInitiateAuth(const FString& Email, const FString& AWSCognitoClientID);

	void CognitoIdentitySignUp(const FString& Email, const FString& Password, const FString& AWSCognitoClientID);

	void AdminRespondToAuthChallenge(const FString& Email, const FString& Answer, const FString& ChallengeSessionString, const FString& AWSCognitoClientID);

	//Sequence Specific//

	void AuthWithSequence(const FString& IDTokenIn, const TArray<uint8_t>& Key);

	//Sequence Specific//

	//RPC Calls//

	TSharedPtr<FJsonObject> ResponseToJson(const FString& response);

	void SequenceRPC(const FString& Url, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	void AuthorizedRPC(const FString& Authorization, const FDateTime& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	void RPC(const FString& Url,const FString& AMZTarget,const FString& RequestBody, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
};