#pragma once
#include "SessionCallSignature.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"

class SEQUENCEPLUGIN_API FExplicitSessionCallSignature final : public FSessionCallSignature
{
public:
	explicit FExplicitSessionCallSignature(const uint8& InPermissionIndex, const TSharedPtr<FRSY>& InSessionSignature):
		PermissionIndex(InPermissionIndex), SessionSignature(InSessionSignature)
	{
		Type = ESessionCallSignatureType::Explicit;
	}

	static TArray<uint8> EncodeSignatures();

	uint8 PermissionIndex;
	TSharedPtr<FRSY> SessionSignature;
};
