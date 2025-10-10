#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfLeaf.h"

class SEQUENCEPLUGIN_API FConfigSignedSignerLeaf final : public FConfigLeaf
{
public:
	explicit FConfigSignedSignerLeaf(const FString& InAddress, const int32& InWeight, const TSharedPtr<FSignatureOfLeaf>& InSignature)
	{
		this->Type = EConfigLeafType::SignedSigner;
		this->Address = InAddress;
		this->Weight = InWeight;
		this->Signature = InSignature;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		return Signature->Encode(Weight);
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}

	FString Address;
	int32 Weight;
	TSharedPtr<FSignatureOfLeaf> Signature;
};
