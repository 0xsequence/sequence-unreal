#pragma once
#include "SignatureOfLeaf.h"
#include "SignatureOfSignerLeafHash.h"

class SEQUENCEPLUGIN_API final FSignature : public FSignatureOfLeaf
{
public:
	explicit FSignature(const FString& InAddress, const TSharedPtr<FSignatureOfSignerLeafHash>& InSignature)
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
		this->Address = InAddress;
		this->Signature = InSignature;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		return TArray<uint8>();
	}

private:
	FString Address;
	TSharedPtr<FSignatureOfSignerLeafHash> Signature;
};
