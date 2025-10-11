#pragma once
#include "SignatureOfLeaf.h"

enum ESapientSignatureType
{
	SapientSignature,
	SapientSignatureCompact
};

class SEQUENCEPLUGIN_API FSignatureOfSapientSignerLeaf final : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSapientSignerLeaf(const FString& Address, const TArray<uint8>& Data)
	{
		this->Type = EConfigSignatureType::SignatureOfSapientSigner;
		this->SapientType = ESapientSignatureType::SapientSignature;
		this->Address = Address;
		this->Data = Data;
	}

	virtual TArray<uint8> Encode(const int32& Weight) override
	{
		return TArray<uint8>();
	}

	ESapientSignatureType SapientType;
	FString Address;
	TArray<uint8> Data;
};
