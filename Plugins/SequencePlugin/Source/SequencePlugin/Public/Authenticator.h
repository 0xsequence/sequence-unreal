// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "RequestHandler.h"
#include "Util/Structs/BE_Enums.h"
#include "Types/Wallet.h"
#include "Config/Config.h"
#include "Dom/JsonObject.h"
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
private:
	UPROPERTY()
	int32 projectId = 0;
	UPROPERTY()
	FString rpcServer = "";
	UPROPERTY()
	FString emailRegion = "";
	UPROPERTY()
	FString emailClientId = "";
public:
	FString GetProjectId() const
	{
		FString ret = "";
		ret.AppendInt(projectId);
		return ret;
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
	int64 Created = -1;
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
	FString RPCServer = "";
	UPROPERTY()
	FString Type = "";
	UPROPERTY()
	FString UserId = "";
	UPROPERTY()
	FString Sub = "";
	UPROPERTY()
	bool Registered = false;
public:
	FCredentials_BE(){}
	FCredentials_BE(const FString& RPCServerIn,const FString& ProjectIdIn, const FString& ProjectAccessKeyIn, const FString& SessionPrivateKeyIn, const FString& SessionIdIn, const FString& IdTokenIn, const FString& EmailIn, const FString& WaasVersionIn)
	{
		ProjectId = ProjectIdIn;
		ProjectAccessKey = ProjectAccessKeyIn;
		SessionPrivateKey = SessionPrivateKeyIn;
		SessionId = SessionIdIn;
		IDToken = IdTokenIn;
		Email = EmailIn;
		WaasVersion = WaasVersionIn;
		RPCServer = RPCServerIn;
	}

	void RegisterCredentials(const FString& WalletIn, const FString& EmailIn, const FString& IssuerIn, const FString& TypeIn, const FString& SubIn, const FString& UserIdIn, const int64 CreatedAtIn, const int64 RefreshedAtIn, const int64 ExpiresAtIn)
	{
		WalletAddress = WalletIn;
		Email = EmailIn;
		Issuer = IssuerIn;
		Type = TypeIn;
		Sub = SubIn;
		UserId = UserIdIn;
		Created = CreatedAtIn;
		Refreshed = RefreshedAtIn;
		Expires = ExpiresAtIn;
		Registered = true;
	}

	FString GetRPCServer() const
	{
		return RPCServer;
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

	int64 GetCreated() const
	{
		return Created;
	}

	int64 GetRefreshed() const
	{
		return Refreshed;
	}

	int64 GetExpires() const
	{
		return Expires;
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
		Expires = -1;
		Created = -1;
		Refreshed = -1;
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

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API UAuthenticator : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FOnAuthRequiresCode AuthRequiresCode;
	UPROPERTY()
	FOnAuthFailure AuthFailure;
	UPROPERTY()
	FOnAuthSuccess AuthSuccess;

private://Broadcast handlers
	void CallAuthRequiresCode() const;
	void CallAuthFailure() const;
	void CallAuthSuccess(const FCredentials_BE& Credentials) const;
//vars
private:
	const FString SaveSlot = "Cr";
	const uint32 UserIndex = 0;

	UPROPERTY()
	FString SessionId = "";
	UPROPERTY()
	FString SessionHash = "";
	UPROPERTY()
	FString StateToken = "";
	UPROPERTY()
	FString Nonce = "";

	UPROPERTY()
	FString Cached_IDToken;
	UPROPERTY()
	FString Cached_Email;
	UPROPERTY()
	FString Cached_Issuer;
	UPROPERTY()
	FWaasJWT WaasSettings;
	
	const TMap<ESocialSigninType, FSSOCredentials> SSOProviderMap = { {ESocialSigninType::Discord,FSSOCredentials(FAuthenticatorConfig::DiscordAuthURL,FAuthenticatorConfig::DiscordClientID)},{ESocialSigninType::FaceBook,FSSOCredentials(FAuthenticatorConfig::FacebookAuthURL,FAuthenticatorConfig::FacebookClientID)},{ESocialSigninType::Apple,FSSOCredentials(FAuthenticatorConfig::AppleAuthURL,FAuthenticatorConfig::AppleClientID)},{ESocialSigninType::Google,FSSOCredentials(FAuthenticatorConfig::GoogleAuthURL,FAuthenticatorConfig::GoogleClientID)}};
	UPROPERTY()
	TArray<FString> PWCharList = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","0","1","2","3","4","5","6","7","8","9"};
	const int32 EmailAuthMaxRetries = 2;
	UPROPERTY()
	int32 EmailAuthCurrRetries = EmailAuthMaxRetries;
	UPROPERTY()
	FString ChallengeSession = "";
	UPROPERTY()
	UWallet * SessionWallet;
	UPROPERTY()
	bool PurgeCache = true;
private:
	UAuthenticator();
public:
	FString GetSigninURL(const ESocialSigninType& Type);

	void InitiateMobileSSO(const ESocialSigninType& Type);

	void UpdateMobileLogin(const FString& TokenizedUrl);
	
	void SocialLogin(const FString& IDTokenIn);

	void EmailLogin(const FString& EmailIn);

	void EmailLoginCode(const FString& CodeIn);

	FStoredCredentials_BE GetStoredCredentials() const;

	void StoreCredentials(const FCredentials_BE& Credentials) const;

	void ClearStoredCredentials() const;
private:
	bool GetStoredCredentials(FCredentials_BE * Credentials) const;

	static bool CredentialsValid(const FCredentials_BE& Credentials);
	
	FString GetISSClaim(const FString& JWT) const ;

	bool CanRetryEmailLogin();

	void ResetRetryEmailLogin();

	FString GenerateSigninURL(const ESocialSigninType& Type) const;

	FString GenerateRedirectURL(const ESocialSigninType& Type) const;

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