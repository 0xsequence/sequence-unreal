#pragma once
#include "SignatureOfLeaf.h"

enum ESapientSignatureType
{
	SapientSignature,
	SapientSignatureCompact
};

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafHash : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafHash(const ESapientSignatureType& SapientType)
	{
		this->Type = EConfigSignatureType::SignatureOfSapientSigner;
		this->SapientType = SapientType;
	}

	virtual TArray<uint8> Encode(const int32& Weight) override
	{
		return TArray<uint8>();
	}

	ESapientSignatureType SapientType;
	FString Address;
	TArray<uint8> Data;
};
