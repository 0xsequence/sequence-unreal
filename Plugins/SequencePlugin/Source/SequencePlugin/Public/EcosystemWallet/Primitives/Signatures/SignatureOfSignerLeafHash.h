#pragma once
#include "SignatureOfLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "Util/ValueUtils.h"
#include "Util/ByteArrayUtils.h"

class SEQUENCEPLUGIN_API FSignatureOfSignerLeafHash : public FSignatureOfLeaf
{
public:
	explicit FSignatureOfSignerLeafHash()
	{
		this->Type = EConfigSignatureType::SignatureOfSigner;
	}

	virtual TArray<uint8> Encode(const FString& Weight) override
	{
		const int32 WeightInt = FValueUtils::StringToInt32(Weight);
		TArray<uint8> WeightBytes;

		// 2. Build the flag
		int32 Flag = FConfigTopology::FlagSignatureHash << 4;

		if (WeightInt > 0 && WeightInt <= 15)
		{
			Flag |= WeightInt;
		}
		else if (WeightInt <= 255)
		{
			WeightBytes = FByteArrayUtils::ByteArrayFromNumber(WeightInt, FByteArrayUtils::MinBytesFor(WeightInt));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Weight too large"));
			checkNoEntry();
		}

		TArray<uint8> FlagBytes = FByteArrayUtils::ByteArrayFromNumber(Flag, FByteArrayUtils::MinBytesFor(Flag));
		TArray<uint8> RsyPacked = Signature->Pack();

		return FByteArrayUtils::ConcatBytes({FlagBytes, WeightBytes, RsyPacked});
	}

	FString Address;
	TSharedPtr<FRSY> Signature;
};
