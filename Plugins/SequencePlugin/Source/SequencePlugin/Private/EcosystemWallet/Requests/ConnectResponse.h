#pragma once

#include "ConnectResponse.generated.h"

USTRUCT()
struct FConnectResponseData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	FString WalletAddress;
	
	UPROPERTY()
	FString LoginMethod;
	
	UPROPERTY()
	FString Attestation;
	
	UPROPERTY()
	FString Signature;
	
	UPROPERTY()
	FString UserEmail;
};

USTRUCT()
struct FConnectResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	bool Result;
	
	UPROPERTY()
	FConnectResponseData Data;
};
