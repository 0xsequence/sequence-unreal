#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"
#include "EcosystemWallet/Primitives/Config/ConfigTopology.h"

class SEQUENCEPLUGIN_API FConfigNestedLeaf final : public FConfigLeaf
{
public:
	FConfigNestedLeaf(const FString& Weight, const FString& Threshold, const TSharedPtr<FConfigTopology>& Tree)
	{
		this->Weight = Weight;
		this->Threshold = Threshold;
		this->Tree = Tree;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		return FConfigLeaf::Encode(NoChainId, CheckpointerData);
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}

	FString Weight;
	FString Threshold;
	TSharedPtr<FConfigTopology> Tree;
};
