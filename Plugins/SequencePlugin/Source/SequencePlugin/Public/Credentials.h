// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Types/Wallet.h"
#include "Credentials.generated.h"

USTRUCT()
struct SEQUENCEPLUGIN_API FWaasJWT
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
struct SEQUENCEPLUGIN_API FCredentials_BE
{
    GENERATED_USTRUCT_BODY()
private:
	UPROPERTY()
    FString SessionPrivateKey = "";
	UPROPERTY()
	FString IDToken = "";
	UPROPERTY()
	FString Email = "";
	UPROPERTY()
    FString WalletAddress = "";
	UPROPERTY()
    FString Issuer = "";
	UPROPERTY()
	FString Type = "";
	UPROPERTY()
	FString Sub = "";
	UPROPERTY()
	FString UserId = "";
	UPROPERTY()
	int64 Created = -1;
	UPROPERTY()
	int64 Refreshed = -1;
	UPROPERTY()
	int64 Expires = -1;
	UPROPERTY()
	int64 Network = 137;
	UPROPERTY()
	bool Registered = false;
public:
	FCredentials_BE(){}

	FCredentials_BE(const FString& SessionPrivateKeyIn, const FString& IdTokenIn,
					const FString& EmailIn, const FString& WalletIn, const FString& IssuerIn,
					const FString& TypeIn, const FString& SubIn, const FString& UserIdIn,
					const int64 CreatedAtIn, const int64 RefreshedAtIn, const int64 ExpiresAtIn)
	{
		SessionPrivateKey = SessionPrivateKeyIn;
		IDToken = IdTokenIn;//Not used in Email Auth
		Email = EmailIn;
		WalletAddress = WalletIn;
		Issuer = IssuerIn;//Not used in Email Auth
		Type = TypeIn;
		Sub = SubIn;
		UserId = UserIdIn;
		Created = CreatedAtIn;
		Refreshed = RefreshedAtIn;
		Expires = ExpiresAtIn;		
		Registered = true;
	}
	
	void UpdateNetwork(int64 NewNetwork)
	{
		Network = NewNetwork;
	}
	
	FString GetNetworkString() const
	{
		return FString::Printf(TEXT("%lld"),Network);
	}
	
	int64 GetNetwork() const
	{
		return Network;
	}

	UWallet * GetSessionWallet() const
	{
		return UWallet::Make(SessionPrivateKey);
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

	//Used to check cases where we are registered and in a good state
	bool RegisteredValid() const
	{
		bool IsValidRegistered = true;
		IsValidRegistered &= Registered;
		IsValidRegistered &= Expires > FDateTime::UtcNow().ToUnixTimestamp();
		//IsValidRegistered &= IDToken.Len() > 0; @TODO check for Per type validation but this is a stop gap
		IsValidRegistered &= SessionPrivateKey.Len() > 0;
		return IsValidRegistered;
	}

	//Used to check in cases where we don't care if we are registered, just valid
	bool UnRegisteredValid() const
	{
		bool IsValid = true;
		IsValid &= IDToken.Len() > 0;
		IsValid &= SessionPrivateKey.Len() > 0;
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
struct SEQUENCEPLUGIN_API FStoredCredentials_BE
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