#pragma once
#include "EcosystemWallet/Primitives/Sessions/SessionsLeaf.h"
#include "Util/ByteArrayUtils.h"

class SEQUENCEPLUGIN_API FSessionsNodeLeaf final : public FSessionsLeaf
{
public:
	explicit FSessionsNodeLeaf(const FString& InValue)
	{
		this->Type = ESessionsLeafType::SessionsNode;
		this->Value = InValue;
	}
	
	virtual TArray<uint8> Encode() override
	{
		const uint32 Flag = FSessionsTopology::FlagNode << 4;
		return FByteArrayUtils::ConcatBytes({ FByteArrayUtils::ByteArrayFromNumber(Flag, 1), FByteArrayUtils::HexStringToBytes(Value)} );
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FByteArrayUtils::HexStringToBytes(Value);
	}

	FString Value;
};
