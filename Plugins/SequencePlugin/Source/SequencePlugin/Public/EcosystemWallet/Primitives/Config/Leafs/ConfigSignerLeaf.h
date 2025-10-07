#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"

class SEQUENCEPLUGIN_API FConfigSignerLeaf final : public FConfigLeaf
{
public:
	explicit FConfigSignerLeaf(const FString& Address, const FString& Weight)
	{
		this->Type = EConfigLeafType::Signer;
		this->Address = Address;
		this->Weight = Weight;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		return FConfigLeaf::Encode(NoChainId, CheckpointerData);
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}

	FString Address;
	FString Weight;
};
