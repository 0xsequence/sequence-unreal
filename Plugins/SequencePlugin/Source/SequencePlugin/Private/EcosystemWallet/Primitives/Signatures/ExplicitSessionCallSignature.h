#pragma once
#include "SessionCallSignature.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FExplicitSessionCallSignature final : public FSessionCallSignature
{
public:
	explicit FExplicitSessionCallSignature(const FBigInt& InPermissionIndex, const FRSY& InSessionSignature):
		Type(ESessionCallSignatureType::Explicit), PermissionIndex(InPermissionIndex), SessionSignature(InSessionSignature) { }

	static TArray<uint8> EncodeSignatures();

	ESessionCallSignatureType Type;
	FBigInt PermissionIndex;
	FRSY SessionSignature;
};
