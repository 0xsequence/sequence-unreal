#include "Relayer/SequenceRelayer.h"

#include "Relayer/Models/GetMetaTxnReceiptArgs.h"
#include "Sequence/SequenceSdk.h"
#include "Util/HttpHandler.h"

void USequenceRelayer::Relay(const FString& To, const FString& Data, const FString& Quote, const TArray<FIntentPrecondition>& Preconditions, TSuccessCallback<FString> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const FMetaTxn Call = {
		To,
		To,
		Data
	};
	
	const FSendMetaTxnArgs Args = {
		Call
	};

	TSuccessCallback<FSendMetaTxnResponse> OnTxnSuccess = [OnSuccess](const FSendMetaTxnResponse& Response)
	{
		OnSuccess(Response.TxnHash);	
	};
	
	this->SendMetaTxn(Args, OnTxnSuccess, OnFailure);
}

void USequenceRelayer::GetFeeOptions(const FFeeOptionsArgs& Args, TSuccessCallback<FFeeOptionsResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	TSuccessCallback<FString> OnRawSuccess = [OnSuccess](const FString& Response)
	{
		const FFeeOptionsResponse Data = USequenceSupport::JSONStringToStruct<FFeeOptionsResponse>(Response);
		OnSuccess(Data);
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	this->SendRequest("FeeOptions", Payload, OnRawSuccess, OnFailure);
}

void USequenceRelayer::GetMetaTxnReceipt(const FString& MetaTxID, TSuccessCallback<FGetMetaTxnReceiptResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	const FGetMetaTxnReceiptArgs Args = {
		MetaTxID,
	};
	
	TSuccessCallback<FString> OnRawSuccess = [OnSuccess](const FString& Response)
	{
		const FGetMetaTxnReceiptResponse Data = USequenceSupport::JSONStringToStruct<FGetMetaTxnReceiptResponse>(Response);
		OnSuccess(Data);
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	this->SendRequest("GetMetaTxnReceipt", Payload, OnRawSuccess, OnFailure);
}

void USequenceRelayer::SendMetaTxn(const FSendMetaTxnArgs& Args, TSuccessCallback<FSendMetaTxnResponse> OnSuccess, const TFunction<void(FString)>& OnFailure) const
{
	TSuccessCallback<FString> OnRawSuccess = [OnSuccess](const FString& Response)
	{
		const FSendMetaTxnResponse Data = USequenceSupport::JSONStringToStruct<FSendMetaTxnResponse>(Response);
		OnSuccess(Data);
	};

	const FString Payload = USequenceSupport::StructToString(Args);
	this->SendRequest("SendMetaTxn", Payload, OnRawSuccess, OnFailure);
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
			Name = "v3-arbitrum";
			break;
		case 421614:
			Name = "v3-arbitrum-sepolia";
			break;
		default: ;
	}

	return FString::Printf(TEXT("https://%s-relayer.sequence.app/rpc/Relayer"), *Name);
}

