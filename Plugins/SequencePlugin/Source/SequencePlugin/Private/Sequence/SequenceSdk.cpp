#include "Sequence/SequenceSdk.h"

#include "Util/ChainCollection.h"
#include "Util/SequenceSupport.h"

void SequenceSdk::SetChainId(FString NewChainId)
{
	if (!FChainCollection::IsNetworkIdSupported(NewChainId))
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
	
FString SequenceSdk::GetChainId()
{
	return ChainId;
}

FString SequenceSdk::GetChainName()
{
	return FChainCollection::GetNetworkName(ChainId);
}

FString SequenceSdk::GetRedirectUrl()
{
	return RedirectUrl;
}

