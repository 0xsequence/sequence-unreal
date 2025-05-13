#include "Sequence/SequenceSdk.h"
#include "Util/SequenceSupport.h"

void SequenceSdk::SetChainId(int64 NewChainId)
{
	ChainId = NewChainId;
}

void SequenceSdk::SetRedirectUrl(const FString& NewRedirectUrl)
{
	RedirectUrl = NewRedirectUrl;
}
	
int64 SequenceSdk::GetChainId()
{
	return ChainId;
}

FString SequenceSdk::GetChainIdString()
{
	return FString::FromInt(ChainId);
}

FString SequenceSdk::GetChainName()
{
	return USequenceSupport::GetNetworkName(ChainId);
}

FString SequenceSdk::GetRedirectUrl()
{
	return RedirectUrl;
}

