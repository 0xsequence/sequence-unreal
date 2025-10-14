#pragma once
#include "SignatureOfLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafEthSign : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafEthSign(const FString& Weight, const TSharedPtr<FRSY>& Signature)
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		return TArray<uint8>();
	}

	TSharedPtr<FRSY> Signature;
};
