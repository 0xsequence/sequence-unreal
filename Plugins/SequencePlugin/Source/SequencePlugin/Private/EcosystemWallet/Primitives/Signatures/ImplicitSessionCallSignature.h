#pragma once
#include "SessionCallSignature.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FImplicitSessionCallSignature final : public FSessionCallSignature
{
public:
	explicit FImplicitSessionCallSignature(const FString& InAttestation, const FRSY& InIdentitySignature, const FRSY& InSessionSignature):
		Type(ESessionCallSignatureType::Implicit), Attestation(InAttestation), IdentitySignature(InIdentitySignature), SessionSignature(InSessionSignature) { }

	ESessionCallSignatureType Type;
	FString Attestation;
	FRSY IdentitySignature;
	FRSY SessionSignature;
};
