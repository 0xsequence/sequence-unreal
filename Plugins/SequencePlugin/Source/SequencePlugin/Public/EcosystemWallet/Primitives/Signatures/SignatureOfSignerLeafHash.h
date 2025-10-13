#pragma once
#include "SignatureOfLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafHash : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafHash()
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		return TArray<uint8>();
	}

	TSharedPtr<FRSY> Signature;
};
