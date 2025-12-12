#pragma once
#include "SessionCallSignature.h"
#include "EcosystemWallet/Primitives/Attestation/Attestation.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"

class SEQUENCEPLUGIN_API FImplicitSessionCallSignature final : public FSessionCallSignature
{
public:
	explicit FImplicitSessionCallSignature(const TSharedPtr<FAttestation>& InAttestation, const TSharedPtr<FRSY>& InIdentitySignature, const TSharedPtr<FRSY>& InSessionSignature):
		Attestation(InAttestation), IdentitySignature(InIdentitySignature), SessionSignature(InSessionSignature)
	{
		Type = ESessionCallSignatureType::Implicit;
	}

	TSharedPtr<FAttestation> Attestation;
	TSharedPtr<FRSY> IdentitySignature;
	TSharedPtr<FRSY> SessionSignature;
};
