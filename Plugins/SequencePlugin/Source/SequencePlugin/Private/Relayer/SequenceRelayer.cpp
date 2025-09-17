#include "Relayer/SequenceRelayer.h"
#include "Sequence/SequenceSdk.h"
#include "Util/HttpHandler.h"

void USequenceRelayer::Relay(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	
}

void USequenceRelayer::GetFeeOptions(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	
}

void USequenceRelayer::GetMetaTxnReceipt(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	
}

void USequenceRelayer::SendMetaTxn(const FString& WalletAddress, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	
}

void USequenceRelayer::SendRequest(const FString& Endpoint, const FString& Payload, TSuccessCallback<FString> OnSuccess, TFunction<void(FString)> OnFailure) const
{
	UHttpHandler* Handler = NewObject<UHttpHandler>();
	Handler->SetRequestUrl(this->BuildUrl());
	Handler->SendPostRequest(Endpoint, Payload, OnSuccess, OnFailure);
}

FString USequenceRelayer::BuildUrl()
{
	const int64 ChainId = SequenceSdk::GetChainId();

	FString Name = "";
	switch (ChainId)
	{
		case 10:
			Name = "v3-optimism";
			break;
		case 42161:
			Name = "dev-arbitrum";
			break;
		case 421614:
			Name = "dev-arbitrum-sepolia";
			break;
		default: ;
	}

	return FString::Printf(TEXT("https://%s-relayer.sequence.app/rpc/Relayer"), *Name);
}

