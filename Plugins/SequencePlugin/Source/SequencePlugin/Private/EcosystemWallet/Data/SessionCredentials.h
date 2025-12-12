#pragma once

struct FSessionCredentials
{
public:
	bool IsImplicit;
	FString& PrivateKeyHex;
	FString& WalletAddress;
	FString& ChainIdStr;
	FString& LoginMethod;
	int32 EcosystemAsInt;
	TOptional<FString>& Email;
	TOptional<FString>& Attestation;
	TOptional<FString>& Signature;
};
