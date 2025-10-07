#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"

class SEQUENCEPLUGIN_API FConfigAnyAddressSubdigestLeaf final : public FConfigLeaf
{
public:
	explicit FConfigAnyAddressSubdigestLeaf(const FString& InSubdigest)
	{
		this->Subdigest = InSubdigest;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		return FConfigLeaf::Encode(NoChainId, CheckpointerData);
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}
	
	FString Subdigest;
};
