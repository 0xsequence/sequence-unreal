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
		int32 Flag = FConfigTopology::FlagAddress << 4;

		TArray<uint8> WeightBytes;
		int32 WeightInt = FValueUtils::StringToInt32(Weight);

		if (WeightInt > 0 && WeightInt <= 15)
		{
			Flag |= WeightInt;
		}
		else if (WeightInt <= 255)
		{
			WeightBytes = FByteArrayUtils::ByteArrayFromNumber(WeightInt, FByteArrayUtils::MinBytesFor(Flag));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Weight too large"));
		}

		TArray<uint8> FlagBytes = FByteArrayUtils::ByteArrayFromNumber(Flag, FByteArrayUtils::MinBytesFor(Flag));

		TArray<uint8> AddressBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::HexStringToBytes(Address), 20);

		return FByteArrayUtils::ConcatBytes({FlagBytes, WeightBytes, AddressBytes});
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		return FConfigLeaf::HashConfiguration();
	}

	FString Address;
	FString Weight;
};
