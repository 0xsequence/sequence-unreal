#pragma once

#include "ConnectResponse.generated.h"

USTRUCT()
struct FConnectResponseData
{
	GENERATED_USTRUCT_BODY()

	FString WalletAddress;
	FString LoginMethod;
	FString Attestation;
	FString Signature;
	FString Email;
};

USTRUCT()
struct FConnectResponse
{
	GENERATED_USTRUCT_BODY()

	bool Result;
	FConnectResponseData Data;
};
