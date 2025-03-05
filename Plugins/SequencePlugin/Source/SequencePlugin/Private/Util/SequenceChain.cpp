#include "Util/SequenceChain.h"

#include "Util/SequenceSupport.h"

void SequenceChain::SetChainId(int64 NewChainId)
{
	ChainId = NewChainId;
}
	
int64 SequenceChain::GetChainId()
{
	return ChainId;
}

FString SequenceChain::GetChainName()
{
	return USequenceSupport::GetNetworkName(ChainId);
}
