// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.


#include "Subsystems/SequenceSdkBP.h"

#include "ConfigFetcher.h"
#include "Util/SequenceChain.h"
#include "Util/SequenceSupport.h"

USequenceSdkBP::USequenceSdkBP()
{
	const FString& DefaultChainName = UConfigFetcher::GetConfigVar("DefaultChain");
	SetChainByName(DefaultChainName);
}

void USequenceSdkBP::SetChainById(const int64 NewChainId)
{
	SequenceChain::SetChainId(NewChainId);
}

void USequenceSdkBP::SetChainByName(const FString& NewChainName)
{
	SequenceChain::SetChainId(USequenceSupport::GetNetworkId(NewChainName));
}

void USequenceSdkBP::SetChainByType(const ENetwork& NewChainType)
{
	SequenceChain::SetChainId(USequenceSupport::GetNetworkId(NewChainType));
}

int64 USequenceSdkBP::GetChainId()
{
	return SequenceChain::GetChainId();
}
