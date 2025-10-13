#pragma once
#include "SignatureOfLeaf.h"
#include "SignatureOfSapientSignerLeaf.h"

class SEQUENCEPLUGIN_API FSapientSignature final : public FSignatureOfLeaf
{
public:
	explicit FSapientSignature(const FString& InImageHash, const TSharedPtr<FSignatureOfSapientSignerLeaf>& InSignature)
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
		this->ImageHash = InImageHash;
		this->Signature = InSignature;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		return TArray<uint8>();
	}

private:
	FString ImageHash;
	TSharedPtr<FSignatureOfSapientSignerLeaf> Signature;
};
