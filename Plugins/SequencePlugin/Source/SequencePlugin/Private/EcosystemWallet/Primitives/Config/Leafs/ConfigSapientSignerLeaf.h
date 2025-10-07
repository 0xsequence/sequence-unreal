#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"

class SEQUENCEPLUGIN_API FConfigSapientSignerLeaf final : public FConfigLeaf
{
public:
	explicit FConfigSapientSignerLeaf(const FString& Address, const FString& Weight, const FString& ImageHash)
	{
		this->Address = Address;
		this->Weight = Weight;
		this->ImageHash = ImageHash;
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
	FString ImageHash;
};
