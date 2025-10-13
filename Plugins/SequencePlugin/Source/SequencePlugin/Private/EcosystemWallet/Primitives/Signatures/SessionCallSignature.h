#pragma once
#include "EcosystemWallet/Primitives/Sessions/SessionsTopology.h"

enum ESessionCallSignatureType
{
	Implicit,
	Explicit,
};

class SEQUENCEPLUGIN_API FSessionCallSignature
{
public:
	virtual ~FSessionCallSignature() = default;
	
	static TArray<uint8> EncodeSignatures(
		const TArray<TSharedPtr<FSessionCallSignature>>& Signatures,
		const TSharedPtr<FSessionsTopology>& SessionsTopology,
		const TArray<FString>& ImplicitSigners,
		const TArray<FString>& ExplicitSigners);

	ESessionCallSignatureType Type;
};
