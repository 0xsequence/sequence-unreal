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
	FString rpcServer = "";
	UPROPERTY()
	FString emailRegion = "";
	UPROPERTY()
	FString emailClientId = "";
};

USTRUCT()
struct FWaasCredentials
{
	GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
	FString projectId = "";
	UPROPERTY()
	FString rpcServer = "";
	UPROPERTY()
	FString emailRegion = "";
	UPROPERTY()
	FString emailClientId = "";
	UPROPERTY()
	bool EmailEnabled = false;
public:
	FWaasCredentials(){}
	FWaasCredentials(const FWaasJWT& JWT)
	{		
		projectId.AppendInt(JWT.projectId);
		emailRegion = JWT.emailRegion;
		emailClientId = JWT.emailClientId;
		EmailEnabled = (emailClientId.Len() > 0);
		
		if (JWT.rpcServer.EndsWith("/"))
			rpcServer = JWT.rpcServer + "rpc/WaasAuthenticator/RegisterSession";
		else
			rpcServer = JWT.rpcServer + "/rpc/WaasAuthenticator/RegisterSession";
	}
	
	FString GetProjectID() const
	{
		return projectId;
	}

	FString GetRPCServer() const
	{
		return rpcServer;
	}

	FString GetEmailRegion() const
	{
		return emailRegion;
	}

	FString GetEmailClientId() const
	{
		return emailClientId;
	}
};

USTRUCT(BlueprintType)
struct FCredentials_BE
{
    GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
    FString SessionPrivateKey = "";
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
	FCredentials_BE(const FString& ProjectIdIn, const FString& ProjectAccessKeyIn, const FString& SessionPrivateKeyIn, const FString& SessionIdIn, const FString& IdTokenIn, const FString& EmailIn, const FString& WaasVersionIn)
	{
		ProjectId = ProjectIdIn;
		ProjectAccessKey = ProjectAccessKeyIn;
		SessionPrivateKey = SessionPrivateKeyIn;
		SessionId = SessionIdIn;
		IDToken = IdTokenIn;
		Email = EmailIn;
		WaasVersion = WaasVersionIn;
	}

	void RegisterCredentials()
	{
		
	}

	FString GetProjectId() const
	{
		return ProjectId;
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
		UWallet * TWallet = UWallet::Make(SessionPrivateKey);
		FString PublicKeyStr = BytesToHex(TWallet->GetWalletPublicKey().Ptr(),TWallet->GetWalletPublicKey().GetLength()).ToLower();
		return PublicKeyStr;
	}

	FString GetSessionWalletAddress() const
	{
		UWallet * TWallet = UWallet::Make(SessionPrivateKey);
		FString AddressStr = BytesToHex(TWallet->GetWalletAddress().Ptr(), TWallet->GetWalletAddress().GetLength()).ToLower();
		return AddressStr;
	}

	FString SignMessageWithSessionWallet(const TArray<uint8>& Message, const int32 MessageLength) const
	{
		UWallet * TWallet = UWallet::Make(SessionPrivateKey);
		TArray<uint8> SigBytes = TWallet->SignMessage(Message, MessageLength);
		FString Signature = BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
		return Signature;
	}
	
	FString SignMessageWithSessionWallet(const FString& Message) const
	{
		UWallet * TWallet = UWallet::Make(SessionPrivateKey);
		TArray<uint8> SigBytes = TWallet->SignMessage(Message);
		FString Signature = BytesToHex(SigBytes.GetData(), SigBytes.Num()).ToLower();
		return Signature;
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

	bool IsRegistered() const
	{
		return Registered;
	}

	bool Valid() const
	{
		bool IsValid = true;
		IsValid &= Expires > FDateTime::UtcNow().ToUnixTimestamp();
		return IsValid;
	}

	void UnRegisterCredentials()
	{
		Registered = false;
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRPC, FString, Response);

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuthenticator : public UObject
{
	GENERATED_BODY()
private:
	FOnRPC ProcessRPCResponse;
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
	void PrintAll();
private:
	const FString SaveSlot = "Cr";
	const uint32 UserIndex = 0;

	FString SessionId = "";
	FString SessionHash = "";
	FString StateToken = "";
	FString Nonce = "";

	const FString UrlScheme = "powered-by-sequence";
	const FString RedirectURL = "https://3d41-142-115-54-118.ngrok-free.app/";
	const FString BRedirectURL = "some text to check with";

	const FString GoogleAuthURL = "https://accounts.google.com/o/oauth2/auth";
	const FString GoogleClientID = "970987756660-35a6tc48hvi8cev9cnknp0iugv9poa23.apps.googleusercontent.com";

	const FString FacebookAuthURL = "https://www.facebook.com/v18.0/dialog/oauth";
	const FString FacebookClientID = "";//TODO still need this

	const FString DiscordAuthURL = "https://discord.com/api/oauth2/authorize";
	const FString DiscordClientID = "";//TODO still need this

	const FString AppleAuthURL = "https://appleid.apple.com/auth/authorize";
	const FString AppleClientID = "com.horizon.sequence.waas";

	FString Cached_IDToken;
	FString Cached_Email;
	FString Cached_Issuer;
	//AWS
	const FString VITE_SEQUENCE_WAAS_CONFIG_KEY = "eyJwcm9qZWN0SWQiOjIsImVtYWlsUmVnaW9uIjoidXMtZWFzdC0yIiwiZW1haWxDbGllbnRJZCI6IjVncDltaDJmYnFiajhsNnByamdvNzVwMGY2IiwicnBjU2VydmVyIjoiaHR0cHM6Ly9uZXh0LXdhYXMuc2VxdWVuY2UuYXBwIn0=";
	FWaasCredentials WaasCredentials;
	const FString ProjectAccessKey = "EeP6AmufRFfigcWaNverI6CAAAAAAAAAA";//Builder Key
	const FString WaasVersion = "1.0.0";

	//this provider map fails to init so I'm going to swap over to a runtime made map instead due to unreliability
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

	UPROPERTY()
	UWallet * SessionWallet;

	bool PurgeCache = false;
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

	//RPC Calls//
	FString ParseResponse(FHttpResponsePtr Response,bool WasSuccessful);

	void CognitoIdentityInitiateAuth(const FString& Email, const FString& AWSCognitoClientID);
	void ProcessCognitoIdentityInitiateAuth(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful);

	void CognitoIdentitySignUp(const FString& Email, const FString& Password, const FString& AWSCognitoClientID);
	void ProcessCognitoIdentitySignUp(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful);

	void AdminRespondToAuthChallenge(const FString& Email, const FString& Answer, const FString& ChallengeSessionString, const FString& AWSCognitoClientID);
	void ProcessAdminRespondToAuthChallenge(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful);

	//RPC Calls//
	static TSharedPtr<FJsonObject> ResponseToJson(const FString& response);
	void UE_AWS_RPC(const FString& Url, const FString& RequestBody,const FString& AMZTarget,void(UAuthenticator::*Callback)(FHttpRequestPtr Req, FHttpResponsePtr Response, bool bWasSuccessful));
};