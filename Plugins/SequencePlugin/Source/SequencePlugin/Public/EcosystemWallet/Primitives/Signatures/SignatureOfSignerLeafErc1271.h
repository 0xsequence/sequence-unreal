#pragma once
#include "SignatureOfLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafErc1271 : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafErc1271(const FString& Address, const TArray<uint8>& Data)
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
		this->Address = Address;
		this->Data = Data;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		return TArray<uint8>();
	}

	FString Address;
	TArray<uint8> Data;
};
