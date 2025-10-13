#pragma once
#include "AttestationAuthData.h"

class SEQUENCEPLUGIN_API FAttestation
{
public:
	FAttestation() :
		ApprovedSigner(""),
		IdentityType(TArray<uint8>()),
		IssuerHash(TArray<uint8>()),
		AudienceHash(TArray<uint8>()),
		ApplicationData(TArray<uint8>()),
		AuthData(FAttestationAuthData()) { }
	
	TArray<uint8> Encode()
	{
		return TArray<uint8>();
	}

	FString ToJsonString()
	{
		return FString();
	}

	FString ApprovedSigner;
	TArray<uint8> IdentityType;
	TArray<uint8> IssuerHash;
	TArray<uint8> AudienceHash;
	TArray<uint8> ApplicationData;
	FAttestationAuthData AuthData;
};
