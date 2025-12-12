#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"

class SEQUENCEPLUGIN_API FConfigNodeLeaf final : public FConfigLeaf
{
public:
	explicit FConfigNodeLeaf(const FString& InValue)
	{
		this->Type = EConfigLeafType::Node;
		this->Value = InValue;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		constexpr int32 Flag = FConfigTopology::FlagNode << 4;
		return FByteArrayUtils::ConcatBytes({FByteArrayUtils::ByteArrayFromNumber(Flag, 1), FByteArrayUtils::HexStringToBytes(Value)});
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}

	FString Value;
};
