#pragma once
#include "SignatureOfLeaf.h"

enum ESapientSignatureType
{
	Sapient,
	SapientCompact
};

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafHash : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafHash();

	virtual TArray<uint8> Encode(const int32& Weight) override
	{
		return TArray<uint8>();
	}

	ESapientSignatureType Type;
	FString Address;
	TArray<uint8> Data;
};
