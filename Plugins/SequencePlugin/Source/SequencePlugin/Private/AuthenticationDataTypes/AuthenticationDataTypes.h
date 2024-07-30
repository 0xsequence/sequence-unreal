// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AuthenticationDataTypes.generated.h"

//This may be apart of the sequenceAPI rather than UAuthenticator!

UENUM()
enum EIdentityType
{
	Email,
	Guest,
	OIDC,
	PlayFab,
};

USTRUCT()
struct FInitiateAuthResponse
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FString Challenge = "";
	UPROPERTY()
	int32 ExpiresIn = 0;
	UPROPERTY()
	FString IdentityType = "";
	UPROPERTY()
	FString SessionId = "";
};

USTRUCT()
struct FFederateAccount
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FString Answer = "";
	UPROPERTY()
	FString IdentityType = "";
	UPROPERTY()
	FString SessionId = "";
	UPROPERTY()
	FString Verifier = "";
	UPROPERTY()
	FString Wallet = "";
};

USTRUCT()
struct FSequenceAccount
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FString Email = "";
	UPROPERTY()
	FString Id = "";
	UPROPERTY()
	TEnumAsByte<EIdentityType> IdentityType = EIdentityType::Email;
	UPROPERTY()
	FString Issuer = "";
};

USTRUCT()
struct FFederateAccountResponse
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY()
	FSequenceAccount Account;
};