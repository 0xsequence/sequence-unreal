#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"
#include "Util/ByteArrayUtils.h"

class SEQUENCEPLUGIN_API FConfigAnyAddressSubdigestLeaf final : public FConfigLeaf
{
public:
	explicit FConfigAnyAddressSubdigestLeaf(const FString& InSubdigest)
	{
		this->Type = EConfigLeafType::AnyAddressSubdigest;
		this->Subdigest = InSubdigest;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		constexpr int32 Flag = FConfigTopology::FlagSignatureAnyAddressSubdigest << 4;
		return FByteArrayUtils::ConcatBytes({FByteArrayUtils::ByteArrayFromNumber(Flag, 1), FByteArrayUtils::HexStringToBytes(Subdigest)});
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}
	
	FString Subdigest;
};
