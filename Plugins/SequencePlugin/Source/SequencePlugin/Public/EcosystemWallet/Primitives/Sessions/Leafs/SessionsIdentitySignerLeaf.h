#pragma once
#include "EcosystemWallet/Primitives/Sessions/SessionsLeaf.h"

class SEQUENCEPLUGIN_API FSessionsIdentitySignerLeaf final : public FSessionsLeaf
{
public:
	explicit FSessionsIdentitySignerLeaf(const FString& InIdentitySigner)
	{
		this->Type = ESessionsLeafType::IdentitySigner;
		this->IdentitySigner = InIdentitySigner;
	}
	
	virtual TArray<uint8> Encode() override
	{
		return FSessionsLeaf::Encode();
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FSessionsLeaf::EncodeForHash();
	}

	FString IdentitySigner;
};
