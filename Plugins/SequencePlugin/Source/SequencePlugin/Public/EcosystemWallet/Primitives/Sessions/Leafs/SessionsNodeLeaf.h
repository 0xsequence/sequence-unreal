#pragma once
#include "EcosystemWallet/Primitives/Sessions/SessionsLeaf.h"

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
		return FSessionsLeaf::Encode();
	}
	
	virtual TArray<uint8> EncodeForHash() override
	{
		return FSessionsLeaf::EncodeForHash();
	}

	FString Value;
};
