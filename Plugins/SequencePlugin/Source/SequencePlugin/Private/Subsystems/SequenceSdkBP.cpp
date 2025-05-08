

#include "Subsystems/SequenceSdkBP.h"
#include "ConfigFetcher.h"
#include "Sequence/SequenceSdk.h"
#include "Util/SequenceSupport.h"

USequenceSdkBP::USequenceSdkBP()
{
	const FString& DefaultChainName = UConfigFetcher::GetConfigVar(UConfigFetcher::DefaultChain);
	SetChainById(FCString::Atoi64(*DefaultChainName));
	SetRedirectUrl("https://api.sequence.app");
}

void USequenceSdkBP::SetChainById(const int64 NewChainId)
{
	SequenceSdk::SetChainId(NewChainId);
}

void USequenceSdkBP::SetChainByName(const FString& NewChainName)
{
	SequenceSdk::SetChainId(USequenceSupport::GetNetworkId(NewChainName));
}

void USequenceSdkBP::SetChainByType(const ENetwork& NewChainType)
{
	SequenceSdk::SetChainId(USequenceSupport::GetNetworkId(NewChainType));
}

void USequenceSdkBP::SetRedirectUrl(const FString& NewRedirectUrl)
{
	SequenceSdk::SetRedirectUrl(NewRedirectUrl);
}

int64 USequenceSdkBP::GetChainId()
{
	return SequenceSdk::GetChainId();
}
