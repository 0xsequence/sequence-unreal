#include "Sequence/SequenceSdk.h"
#include "Util/SequenceSupport.h"

void SequenceSdk::SetChainId(int64 NewChainId)
{
	if (!USequenceSupport::IsNetworkIdSupported(NewChainId))
	{
		SEQ_LOG(Error, TEXT("Invalid ChainId when calling 'SequenceSdk::SetChainId(int64 NewChainId)'"));
		return;
	}
	
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

