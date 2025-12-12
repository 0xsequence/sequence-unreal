#pragma once

enum EConfigSignatureType
{
	SignatureOfSigner,
	SignatureOfSapientSigner
};

class SEQUENCEPLUGIN_API FSignatureOfLeaf
{
public:
	virtual ~FSignatureOfLeaf() = default;

	virtual TArray<uint8> Encode(const FString& Weight)
	{
		return TArray<uint8>();
	}

	EConfigSignatureType Type;
};
