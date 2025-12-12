#pragma once
#include "EcosystemWallet/Primitives/Sessions/SessionsLeaf.h"
#include "Util/ByteArrayUtils.h"

class SEQUENCEPLUGIN_API FSessionsIdentitySignerLeaf final : public FSessionsLeaf
{
public:
	explicit FSessionsIdentitySignerLeaf(const FString& InIdentitySigner)
	{
		this->Type = ESessionsLeafType::IdentitySigner;
		this->IdentitySigner = InIdentitySigner;
	}
	
	virtual TArray<uint8> Encode() override
	{
		constexpr uint32 Flag = FSessionsTopology::FlagIdentitySigner << 4;
		return FByteArrayUtils::ConcatBytes({ FByteArrayUtils::ByteArrayFromNumber(Flag, 1), FByteArrayUtils::HexStringToBytes(IdentitySigner) });
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FByteArrayUtils::ConcatBytes({ FByteArrayUtils::ByteArrayFromNumber(FSessionsTopology::FlagIdentitySigner, 1), FByteArrayUtils::HexStringToBytes(IdentitySigner) });
	}

	FString IdentitySigner;
};
