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
		return FSessionsLeaf::Encode();
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FSessionsLeaf::EncodeForHash();
	}

	TArray<FString> Blacklist;
};
