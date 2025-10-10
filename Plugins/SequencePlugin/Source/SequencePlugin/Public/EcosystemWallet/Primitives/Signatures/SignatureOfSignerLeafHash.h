#pragma once
#include "SignatureOfLeaf.h"

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafHash : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafHash()
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
	}

	virtual TArray<uint8> Encode(const int32& Weight) override
	{
		return TArray<uint8>();
	}
};
