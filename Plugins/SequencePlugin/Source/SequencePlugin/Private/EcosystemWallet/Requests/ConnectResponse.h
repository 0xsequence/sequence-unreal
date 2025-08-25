#pragma once

#include "ConnectResponse.generated.h"

USTRUCT()
struct FConnectResponseData
{
	GENERATED_USTRUCT_BODY()

	FString WalletAddress;
	FString LoginMethod;
	TOptional<FString> Attestation;
	TOptional<FString> Signature;
	TOptional<FString> Email;
};

USTRUCT()
struct FConnectResponse
{
	GENERATED_USTRUCT_BODY()

	bool Result;
	FConnectResponseData Data;
};
