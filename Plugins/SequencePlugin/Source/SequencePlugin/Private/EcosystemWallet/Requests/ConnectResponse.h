#pragma once

struct FConnectResponseData
{
	FString WalletAddress;
	FString LoginMethod;
	TOptional<FString> Attestation;
	TOptional<FString> Signature;
	TOptional<FString> Email;
};


struct FConnectResponse
{
	bool Result = false;
	FConnectResponseData Data;
};
