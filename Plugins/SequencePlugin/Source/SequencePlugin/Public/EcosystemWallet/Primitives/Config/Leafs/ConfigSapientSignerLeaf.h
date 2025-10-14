#pragma once

#include "EcosystemWallet/Primitives/Config/ConfigLeaf.h"
#include "Util/ValueUtils.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"

class SEQUENCEPLUGIN_API FConfigSapientSignerLeaf final : public FConfigLeaf
{
public:
	explicit FConfigSapientSignerLeaf(const FString& Address, const FString& Weight, const FString& ImageHash)
	{
		this->Type = EConfigLeafType::SapientSigner;
		this->Address = Address;
		this->Weight = Weight;
		this->ImageHash = ImageHash;
	}
	
	virtual TArray<uint8> Encode(const bool NoChainId, const TArray<uint8>& CheckpointerData) override
	{
		constexpr int32 Flag = FConfigTopology::FlagNode << 4;
		return FByteArrayUtils::ConcatBytes({FByteArrayUtils::ByteArrayFromNumber(Flag, FByteArrayUtils::MinBytesFor(Flag)), HashConfiguration()});
	}
	
	virtual TArray<uint8> HashConfiguration() override
	{
		static const FString PrefixStr = TEXT("Sequence sapient config:\n");
		
		TArray<uint8> Prefix;
		FTCHARToUTF8 Converter(*PrefixStr);
		Prefix.Append((const uint8*)Converter.Get(), Converter.Length());

		TArray<uint8> AddressBytes = FByteArrayUtils::HexStringToBytes(Address);

		const int32 WeightInt = FValueUtils::StringToInt32(Weight);
		TArray<uint8> WeightBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::ByteArrayFromNumber(WeightInt, FByteArrayUtils::MinBytesFor(WeightInt)), 32);

		TArray<uint8> ImageHashBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::HexStringToBytes(ImageHash), 32);

		const TArray<uint8> Combined = FByteArrayUtils::ConcatBytes({Prefix, AddressBytes, WeightBytes, ImageHashBytes});

		return FSequenceCoder::KeccakHash(Combined);
	}

	FString Address;
	FString Weight;
	FString ImageHash;
};
