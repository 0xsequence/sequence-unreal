#pragma once
#include "EcosystemWallet/Primitives/Sessions/SessionsLeaf.h"

class SEQUENCEPLUGIN_API FSessionsImplicitBlacklistLeaf final : public FSessionsLeaf
{
public:
	explicit FSessionsImplicitBlacklistLeaf(const TArray<FString>& InBlacklist)
	{
		this->Type = ESessionsLeafType::ImplicitBlacklist;
		this->Blacklist = InBlacklist;
	}
	
	virtual TArray<uint8> Encode() override
	{
		TArray<uint8> EncodedBlacklist = EncodeBlacklist();
		
		int32 BlacklistSize = this->Blacklist.Num();
		if (BlacklistSize >= 0x0f)
		{
			if (BlacklistSize > 0xFFFF)
			{
				UE_LOG(LogTemp, Error, TEXT("Blacklist too large (count=%d)"), BlacklistSize);
				return {};
			}

			constexpr uint64 Flag = (static_cast<uint64>(FSessionsTopology::FlagBlacklist) << 4) | 0x0f;
			TArray<uint8> FlagBytes   = FByteArrayUtils::ByteArrayFromNumber(Flag, 1);
			TArray<uint8> CountBytes  = FByteArrayUtils::ByteArrayFromNumber(BlacklistSize, 1);
			return FByteArrayUtils::ConcatBytes({FlagBytes, CountBytes, EncodedBlacklist});
		}
		
		constexpr uint32 Flag = FSessionsTopology::FlagBlacklist << 4;
		return FByteArrayUtils::ConcatBytes({ FByteArrayUtils::ByteArrayFromNumber(Flag, 1), EncodedBlacklist });
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FByteArrayUtils::ConcatBytes({ FByteArrayUtils::ByteArrayFromNumber(FSessionsTopology::FlagBlacklist, 1), EncodeBlacklist() });
	}

	TArray<FString> Blacklist;

private:
	TArray<uint8> EncodeBlacklist()
	{
		TArray<TArray<uint8>> EncodedAddresses;
		for (FString Address : Blacklist)
		{
			EncodedAddresses.Add(FByteArrayUtils::HexStringToBytes(Address));
		}

		return FByteArrayUtils::ConcatBytes(EncodedAddresses);
	}
};
