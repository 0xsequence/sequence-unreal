#include "Relayer/SequenceRelayerBP.h"

USequenceRelayerBP::USequenceRelayerBP()
{
	this->Relayer = NewObject<USequenceRelayer>();
}

void USequenceRelayerBP::Relay(const FString& To, const FString& Data, const FString& Quote, const TArray<FIntentPrecondition>& Preconditions, FOnRelaySuccess OnSuccess, FOnFailure OnFailure)
{
	TSuccessCallback<FString> OnRawSuccess = [OnSuccess](const FString& TxnHash)
	{
		OnSuccess.ExecuteIfBound(TxnHash);
	};

	TFunction<void(FString)> OnRawSuccessFunction = [OnFailure](const FString& Error)
	{
		OnFailure.ExecuteIfBound(Error);
	};

	this->Relayer->Relay(To, Data, Quote, Preconditions, OnRawSuccess, OnRawSuccessFunction);
}

void USequenceRelayerBP::GetFeeOptions(const FFeeOptionsArgs& Args, FOnGetFeeOptionsSuccess OnSuccess, FOnFailure OnFailure)
{
	TSuccessCallback<FFeeOptionsResponse> OnRawSuccess = [OnSuccess](const FFeeOptionsResponse& Data)
	{
		OnSuccess.ExecuteIfBound(Data);
	};

	TFunction<void(FString)> OnRawSuccessFunction = [OnFailure](const FString& Error)
	{
		OnFailure.ExecuteIfBound(Error);
	};

	this->Relayer->GetFeeOptions(Args, OnRawSuccess, OnRawSuccessFunction);
}

void USequenceRelayerBP::GetMetaTxnReceipt(const FString& MetaTxID, FOnGetMetaTxnReceiptSuccess OnSuccess, FOnFailure OnFailure)
{
	TSuccessCallback<FGetMetaTxnReceiptResponse> OnRawSuccess = [OnSuccess](const FGetMetaTxnReceiptResponse& Data)
	{
		OnSuccess.ExecuteIfBound(Data);
	};

	TFunction<void(FString)> OnRawSuccessFunction = [OnFailure](const FString& Error)
	{
		OnFailure.ExecuteIfBound(Error);
	};

	this->Relayer->GetMetaTxnReceipt(MetaTxID, OnRawSuccess, OnRawSuccessFunction);
}

void USequenceRelayerBP::SendMetaTxn(const FSendMetaTxnArgs& Args, FOnSendMetaTxnSuccess OnSuccess, FOnFailure OnFailure)
{
	TSuccessCallback<FSendMetaTxnResponse> OnRawSuccess = [OnSuccess](const FSendMetaTxnResponse& Data)
	{
		OnSuccess.ExecuteIfBound(Data);
	};

	TFunction<void(FString)> OnRawSuccessFunction = [OnFailure](const FString& Error)
	{
		OnFailure.ExecuteIfBound(Error);
	};

	this->Relayer->SendMetaTxn(Args, OnRawSuccess, OnRawSuccessFunction);
}
