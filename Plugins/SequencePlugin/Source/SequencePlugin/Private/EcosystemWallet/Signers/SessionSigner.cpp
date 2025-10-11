#include "SessionSigner.h"

#include "EcosystemWallet/Primitives/Signatures/ExplicitSessionCallSignature.h"

void FSessionSigner::Initialize(const FSessionCredentials& NewCredentials)
{
	this->Credentials = NewCredentials;
	this->Address = NewCredentials.SessionAddress;
	this->IsExplicit = true;
}

FString FSessionSigner::GetIdentitySigner()
{
	if (IsExplicit)
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

