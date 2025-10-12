#include "SessionSigner.h"

#include "EcosystemWallet/Primitives/Signatures/ExplicitSessionCallSignature.h"

FString FSessionSigner::GetIdentitySigner()
{
	if (Credentials.IsExplicit)
	{
		return "";
	}

	return "";
}

void FSessionSigner::IsSupportedCall(const FCall& Call, TFunction<void(bool)> OnCompleted)
{
	OnCompleted(true);
}

FSessionCallSignature FSessionSigner::SignCall(const FBigInt& ChainId, const FCall& Call, const int32& CallIdx,
	const TSharedPtr<FSessionsTopology> SessionsTopology, const FBigInt& Space, const FBigInt& Nonce)
{
	return FSessionCallSignature();
}

