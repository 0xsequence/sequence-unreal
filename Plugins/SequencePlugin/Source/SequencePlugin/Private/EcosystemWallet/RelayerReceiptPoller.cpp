#include "RelayerReceiptPoller.h"
#include "Engine/Engine.h"

void URelayerReceiptPoller::StartPolling(USequenceRelayer& InRelayer, const FString& Hash, const TFunction<void(FString)>& OnSuccess, const TFunction<void(FString)>& OnFailure)
{
	this->Relayer = &InRelayer;
	this->OnSuccess = OnSuccess;
	this->OnFailure = OnFailure;
	
	CurrentStatus = "Pending";
	PollRecursive(Hash);
}

void URelayerReceiptPoller::PollRecursive(const FString& Hash)
{
	Relayer->GetMetaTxnReceipt(Hash, [this, Hash](const FGetMetaTxnReceiptResponse& Response){
		CurrentStatus = Response.Receipt.Status;

		if (CurrentStatus == "SUCCEEDED" || CurrentStatus == "FAILED")
		{
			OnSuccess(Response.Receipt.TxnHash);
			return;
		}

		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Hash]()
		{
			FPlatformProcess::Sleep(2.0f);
			AsyncTask(ENamedThreads::GameThread, [this, Hash]()
			{
				PollRecursive(Hash);
			});
		});
	}, OnFailure);
}