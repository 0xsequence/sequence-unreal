#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfLeaf.h"

class SEQUENCEPLUGIN_API FConfigSignedSapientSignerLeaf final : public FConfigLeaf
{
public:
	explicit FConfigSignedSapientSignerLeaf(const FString& InAddress, const FString& InWeight, const FString& ImageHash, const TSharedPtr<FSignatureOfLeaf>& InSignature)
	{
		this->Type = EConfigLeafType::SignedSapientSigner;
		this->Address = InAddress;
		this->Weight = InWeight;
		this->ImageHash = ImageHash;
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
	FString Weight;
	FString ImageHash;
	TSharedPtr<FSignatureOfLeaf> Signature;
};
