#include "Subsystems/SequenceSdkBP.h"
#include "ConfigFetcher.h"
#include "Sequence/SequenceSdk.h"
#include "Util/ChainCollection.h"
#include "Util/SequenceSupport.h"

USequenceSdkBP::USequenceSdkBP()
{
	const FString& DefaultChainName = UConfigFetcher::GetConfigVar(UConfigFetcher::DefaultChain);
	SetChainById(FCString::Atoi64(*DefaultChainName));
	SetRedirectUrl("https://api.sequence.app");
}

void USequenceSdkBP::SetChainById(const FString NewChainId)
{
	SequenceSdk::SetChainId(NewChainId);
}

void USequenceSdkBP::SetChainByName(const FString& NewChainName)
{
	SequenceSdk::SetChainId(FChainCollection::GetNetworkId(NewChainName));
}

void USequenceSdkBP::SetChainByType(const ENetwork& NewChainType)
{
	SequenceSdk::SetChainId(FChainCollection::GetNetworkId(NewChainType));
}

void USequenceSdkBP::SetRedirectUrl(const FString& NewRedirectUrl)
{
	SequenceSdk::SetRedirectUrl(NewRedirectUrl);
}

FString USequenceSdkBP::GetChainId()
{
	return SequenceSdk::GetChainId();
}
