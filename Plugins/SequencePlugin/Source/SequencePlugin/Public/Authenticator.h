// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

USTRUCT()
struct FWaasJWT
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 projectId = 0;
	UPROPERTY()
	FString identityPoolId = "";
	UPROPERTY()
	FString idpRegion = "";
	UPROPERTY()
	FString rpcServer = "";
	UPROPERTY()
	FString kmsRegion = "";
	UPROPERTY()
	FString KeyId = "";
	UPROPERTY()
	FString alg = "";
	UPROPERTY()
	FString emailClientId = "";
};

struct FWaasCredentials
{
private:
	FString projectId = "";
	FString identityPoolId = "";
	FString idpRegion = "";
	FString rpcServer = "";
	FString kmsRegion = "";
	FString alg = "";
	FString CognitoClientId = "";
	FString KMSArn = "";
	FString KMSKeyId = "";
	bool EmailEnabled = false;
public:
	FWaasCredentials(){}
	FWaasCredentials(const FWaasJWT& JWT)
	{	//parse for the kms key to start right away!
		TArray<FString> KeyParts;
		KMSArn = JWT.KeyId;
		JWT.KeyId.ParseIntoArray(KeyParts, TEXT(":"), true);
		FString KMS = "";
		if (KeyParts.Num() > 0)
			KMS = KeyParts.Last().RightChop(4);
		
		projectId.AppendInt(JWT.projectId);
		identityPoolId = JWT.identityPoolId;
		idpRegion = JWT.idpRegion;
		
		if (JWT.rpcServer.EndsWith("/"))
			rpcServer = JWT.rpcServer + "rpc/WaasAuthenticator/RegisterSession";
		else
			rpcServer = JWT.rpcServer + "/rpc/WaasAuthenticator/RegisterSession";
		
		kmsRegion = JWT.kmsRegion;
		alg = JWT.alg;
		CognitoClientId = JWT.emailClientId;
		KMSKeyId = KMS;
		EmailEnabled = (CognitoClientId.Len() > 0);
	}
	
	FString GetKMSArn() const
	{
		return KMSArn;
	}
	
	FString GetProjectID() const
	{
		return projectId;
	}

	FString GetIdentityPoolId() const
	{
		return identityPoolId;
	}

	FString GetIDPRegion() const
	{
		return idpRegion;
	}

	FString GetRPCServer() const
	{
		return rpcServer;
	}

	FString GetKMSRegion() const
	{
		return kmsRegion;
	}

	FString GetAlg() const
	{
		return alg;
	}

	FString GetCognitoClientId() const
	{
		return CognitoClientId;
	}

	FString GetKMSKeyId() const
	{
		return KMSKeyId;
	}
};

USTRUCT(BlueprintType)
struct FCredentials_BE
{
    GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
    TArray<uint8> TransportKey;
	UPROPERTY()
	FString EncryptedPayloadKey = "";
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
	UPROPERTY()
	int64 Issued = -1;
	UPROPERTY()
	int64 Refreshed = -1;
	UPROPERTY()
	int64 Expires = -1;
	UPROPERTY()
	FString ProjectAccessKey = "";
	UPROPERTY()
	FString WaasVersion = "";
	UPROPERTY()
	int64 Network = 137;
	UPROPERTY()
	FString ProjectId = "";
	UPROPERTY()
	bool Registered = false;
public:
	FCredentials_BE(){}

    FCredentials_BE(const FString& ProjectIdIn, const TArray<uint8>& TransportKeyIn,const FString& EncryptedPayloadKeyIn,
    	const FString& ProjectAccessKeyIn ,const FString& SessionPrivateKeyIn,
    	const FString& IdIn, const FString& AddressIn,const FString& UserIdIn,
    	const FString& SubjectIn, const FString& SessionIdIn, const FString& WalletAddressIn,
    	const FString& IDTokenIn, const FString& EmailIn, const FString& IssuerIn, const int64& IssuedIn,
    	const int64& RefreshedIn, const int64& ExpiresIn, const FString& WaasVersionIn)
    {
		ProjectId = ProjectIdIn;
		TransportKey = TransportKeyIn;
		EncryptedPayloadKey = EncryptedPayloadKeyIn;
		ProjectAccessKey = ProjectAccessKeyIn;
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
		Issued = IssuedIn;
		Refreshed = RefreshedIn;
		Expires = ExpiresIn;
		WaasVersion = WaasVersionIn;
    }

	FString GetProjectId() const
	{
		return ProjectId;
	}
	
	FString GetEncryptedPayloadKey() const
	{
		return EncryptedPayloadKey;
	}
	
	FString GetNetworkString() const
	{
		return FString::Printf(TEXT("%lld"),Network);
	}
	
	int64 GetNetwork() const
	{
		return Network;
	}
	
	FString GetWaasVersin() const
	{
		return WaasVersion;
	}
	
	TArray<uint8> GetTransportKey() const
	{
		return TransportKey;
	}

	FString GetProjectAccessKey() const
	{
		return ProjectAccessKey;
	}

	FString GetSessionPrivateKey() const
	{
		return SessionPrivateKey;
	}

	FString GetSessionPublicKey() const
	{
		FWallet TWallet = FWallet(SessionPrivateKey);
		FString PublicKeyStr = BytesToHex(TWallet.GetWalletPublicKey().Ptr(),TWallet.GetWalletPublicKey().GetLength()).ToLower();
		return PublicKeyStr;
	}

	FString GetSessionWalletAddress() const
	{
		FWallet TWallet = FWallet(SessionPrivateKey);
		FString AddressStr = BytesToHex(TWallet.GetWalletAddress().Ptr(), TWallet.GetWalletAddress().GetLength()).ToLower();
		return AddressStr;
	}

	FString SignMessageWithSessionWallet(const TArray<uint8>& Message, const int32 MessageLength) const
	{
		FWallet TWallet = FWallet(SessionPrivateKey);
		TArray<uint8> SigBytes = TWallet.SignMessage(Message, MessageLength);
		FString Signature = BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
		return Signature;
	}
	
	FString SignMessageWithSessionWallet(const FString& Message) const
	{
		FWallet TWallet = FWallet(SessionPrivateKey);
		TArray<uint8> SigBytes = TWallet.SignMessage(Message);
		FString Signature = BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
		return Signature;
	}

	FString GetId() const
	{
		return Id;
	}

	FString GetAddress() const
	{
		return Address;
	}

	FString GetUserId() const
	{
		return UserId;
	}

	FString GetSubject() const
	{
		return Subject;
	}

	FString GetSessionId() const
	{
		return SessionId;
	}

	FString GetWalletAddress() const
	{
		return WalletAddress;
	}

	FString GetIDToken() const
	{
		return IDToken;
	}

	FString GetEmail() const
	{
		return Email;
	}

	FString GetIssuer() const
	{
		return Issuer;
	}

	int64 GetIssued() const
	{
		return Issued;
	}

	int64 GetRefreshed() const
	{
		return Refreshed;
	}

	int64 GetExpires() const
	{
		return Expires;
	}

	void RegisterSessionData(const FString& RegisteredSessionId, const FString& RegisteredWallet)
	{
		Registered = true;
		SessionId = RegisteredSessionId;
		WalletAddress = RegisteredWallet;
	}
};

USTRUCT(BlueprintType)
struct FStoredCredentials_BE
{
	GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
	bool Valid = false;
	UPROPERTY()
	FCredentials_BE Credentials;
public:
	FStoredCredentials_BE(){}
	FStoredCredentials_BE(const bool& ValidIn, const FCredentials_BE& CredentialsIn)
	{
		Valid = ValidIn;
		Credentials = CredentialsIn;
	}

	bool GetValid() const
	{
		return Valid;
	}

	FCredentials_BE GetCredentials()
	{
		return Credentials;
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
	void CallAuthRequiresCode() const;
	void CallAuthFailure() const;
	void CallAuthSuccess(const FCredentials_BE& Credentials) const;
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
	const FString VITE_SEQUENCE_WAAS_CONFIG_KEY = "eyJwcm9qZWN0SWQiOjIsImlkZW50aXR5UG9vbElkIjoidXMtZWFzdC0yOjQyZGMyZjE4LTJmOWItNGZkNS05NDI5LWUwZGE1MjFmOWNmNCIsImlkcFJlZ2lvbiI6InVzLWVhc3QtMiIsInJwY1NlcnZlciI6Imh0dHBzOi8vZGV2LXdhYXMuc2VxdWVuY2UuYXBwIiwia21zUmVnaW9uIjoidXMtZWFzdC0yIiwia2V5SWQiOiJhcm46YXdzOmttczp1cy1lYXN0LTI6MzgxNDkyMjQ3Njk3OmtleS8xODgxZTY3My1mMThkLTQ1NTgtODI5YS0xM2I4MThjMDMwNjUifQ";
	FWaasCredentials WaasCredentials;
	const FString ProjectAccessKey = "EeP6AmufRFfigcWaNverI6CAAAAAAAAAA";
	const FString WaasVersion = "1.0.0";

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
	TArray<uint8> PlainTextBytes;
	FString CipherTextBlob = "";

	//From InitiateAuth
	FString ChallengeSession = "";

	//Gonna need to free this in a destructor unless I swap it to a UObject to use garbage collection
	FWallet * SessionWallet;
private:
	UAuthenticator();
public:
	FString GetSigninURL(const ESocialSigninType& Type) const;

	FString GetRedirectURL() const;

	void SocialLogin(const FString& IDTokenIn);

	void EmailLogin(const FString& EmailIn);

	void EmailLoginCode(const FString& CodeIn);

	FStoredCredentials_BE GetStoredCredentials() const;

	void StoreCredentials(const FCredentials_BE& Credentials) const;
private:
	bool GetStoredCredentials(FCredentials_BE * Credentials) const;

	static bool CredentialsValid(const FCredentials_BE& Credentials);
	
	FString GetISSClaim(const FString& JWT) const ;

	bool CanRetryEmailLogin();

	void ResetRetryEmailLogin();

	FString GenerateSigninURL(const FString& AuthURL, const FString& ClientID) const;

	FString BuildAWSURL(const FString& Service, const FString& AWSRegion);

	FString GenerateSignUpPassword();

	FString BuildYYYYMMDD(const FDateTime& Date);

	static FString BuildFullDateTime(const FDateTime& Date);

	FString BuildScope(const FDateTime& Date);

	static FString BuildCanonicalRequest(const FString& URI, const FDateTime& Date, const FString& Payload);//step 1

	FString BuildStringToSign(const FDateTime& Date, const FString& CanonicalRequest);//step 2

	TArray<uint8_t> BuildSigningKey(const FDateTime& Date);//step 3

	static FString BuildSignature(const TArray<uint8_t>& SigningKey, const FString& StringToSign);//step 4

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

	static TSharedPtr<FJsonObject> ResponseToJson(const FString& response);

	void SequenceRPC(const FString& Url, const FString& RequestBody, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;

	void AuthorizedRPC(const FString& Authorization, const FDateTime& Date, const FString& Url, const FString& AMZTarget, const FString& RequestBody, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;

	static void RPC(const FString& Url,const FString& AMZTarget,const FString& RequestBody, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure);
};